#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "map_loader.h"
#include "pso.h"
#include "logger.h"
#include "utils.h"
#include "grafika.h"

void load_config(const char *filename, PSOConfig *cfg) {
    FILE *f = fopen(filename, "r");
    if (f) {
        fscanf(f, "%lf %lf %lf", &cfg->w, &cfg->c1, &cfg->c2);
        fclose(f);
    }
}

int main(int argc, char **argv){
    //standardowe inicjowanie ziarna czasem, aby losowane liczby za kazdym razem byly inne
    srand(time(NULL)); //uruchomienie sdla :()
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Błąd SDL: %s\n", SDL_GetError());
    return 1;
}
    //sprawdzanie czy uzytkownik podal sciezke do pliku z mapa
    if (argc < 2) {
        printf("Uzycie: %s <plik_mapy> [-p n] [-i n] ...\n", argv[0]);
        return 1;
    }

    // wczytywanie mapy
    char *m = argv[1];
    double **mapa = load_map_from_file(m); 
    
    if (mapa == NULL) {
        return 1;
    }
    // jesli uzytkownik nie poda flag przypisywane sa te wartosci (zgodnie z wymaganiami w pliku)
    int particles_count = 100;
    int iterations = 100;
    int log_interval = 0;
    char *config_file = NULL;
    
    //sprawdzanie flag
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) particles_count = atoi(argv[++i]);
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) iterations = atoi(argv[++i]);
        else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) config_file = argv[++i];
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) log_interval = atoi(argv[++i]);
    }
    // domyslne parametry algorytmu: bezwladnosc, pamiec wlasna, wiedza roju
    PSOConfig config = {0.5, 1.0, 1.0};
    // TWORZENIE MAPY!!
    int okn_x = 600;
    int okn_y = 600;
    int map_x = mapa[0][0];
    int map_y = mapa[0][1];
    SDL_Window *okno = SDL_CreateWindow("Mapa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, okn_x, okn_y, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(okno, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *map_tex = stworz_teksture_mapy(renderer, mapa, map_x, map_y, maks_z_mapy(mapa));
    if (config_file) load_config(config_file, &config);
    //inicjalizacja roju
    Swarm *swarm = init_swarm(particles_count, mapa);
    //inicjzalizacja loggera (tylko jesli uztkownik zdecyduje)
    FILE *log_fp = (log_interval > 0) ? init_logger("pso_log.csv") : NULL;

    //GLOWNA PETLA
    int running = 1;
    SDL_Event e;
    int t = 0;
    while(running){
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // czyszczenie rendera
        //zapis danych w odstepie co log_interval krokow NIE ZROBILEM TEGO JESCZE
        if (log_interval > 0 && (t % log_interval == 0)) {
            log_swarm(log_fp, t, swarm);
        }
        rysuj_mape(renderer, map_tex);
        rysuj_swarm(renderer, swarm, map_x, map_y, okn_x, okn_y);
        SDL_RenderPresent(renderer); //wyswietlanie utworzonej klatki
        update_swarm(swarm, mapa, config);
        SDL_Delay(50); //spowolnienie do 60fps
        t++;
    }
    

    //wyswietlanie wyniku koncowego
    printf("X=%.2f Y=%.2f\n", swarm->gBest_x, swarm->gBest_y);
    printf("Val=%.4f\n", swarm->gBest_val);

    //czyszczenie pamieci :)
    if (log_fp) close_logger(log_fp);
    free_swarm(swarm);
    free_loaded_map(mapa);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(okno);
    SDL_Quit();

    return 0;
}