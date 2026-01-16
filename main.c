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
    srand(time(NULL));

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
    int particles_count = 30;
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
    SDL_Window *okno = SDL_CreateWindow("Mapa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, okn_x, okn_y, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(okno, -1, SDL_RENDERER_ACCELERATED);
    //GLOWNA PETLA
    int running = 1;
    SDL_Event e;
    while(running){
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // czyszczenie rendera
        wyswietl_mape(renderer, mapa, okn_x, okn_y, maks_z_mapy(mapa)); //wyswietlanie mapy
        SDL_RenderPresent(renderer); //wyswietlanie utworzonej klatki
        SDL_Delay(16); //spowolnienie do 60fps
    }
    
    if (config_file) load_config(config_file, &config);
    

    //inicjalizacja roju
    Swarm *swarm = init_swarm(particles_count, mapa);

    //inicjzalizacja loggera (tylko jesli uztkownik zdecyduje)
    FILE *log_fp = (log_interval > 0) ? init_logger("pso_log.csv") : NULL;

    //glowna petla symulacji
    for (int t = 0; t < iterations; t++) {
        //wykonywanie obliczen dla wszystkich czastek w roju
        update_swarm(swarm, mapa, config);

        //zapis danych w odstepie co log_interval krokow (jesli uzytkownik taka podal)
        if (log_interval > 0 && (t % log_interval == 0)) {
            log_swarm(log_fp, t, swarm);
        }
    }

    //wyswietlanie wyniku koncowego
    printf("X=%.2f Y=%.2f\n", swarm->gBest_x, swarm->gBest_y);
    printf("Val=%.4f\n", swarm->gBest_val);

    //czyszczenie pamieci :)
    if (log_fp) close_logger(log_fp);
    free_swarm(swarm);
    free_loaded_map(mapa);

    return 0;
}