#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Dodane dla getopt
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
    } else {
        printf("Nie udało się wczytać pliku konfiguracyjnego %s!!\nWczytuję domyślny config...\n", filename);
    }
}

int main(int argc, char **argv) {
    //standardowe inicjowanie ziarna czasem, aby losowane liczby za kazdym razem byly inne
    srand(time(NULL)); //uruchomienie sdla :()
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Błąd SDL: %s\n", SDL_GetError());
        return 1;
    }

    // jesli uzytkownik nie poda flag przypisywane sa te wartosci (zgodnie z wymaganiami w pliku)
    int particles_count = 30;
    int iterations = 1000;
    int log_interval = 0;
    char *config_file = NULL;
    int okn_x = 900;
    int okn_y = 900;
    char *m = NULL;

    //sprawdzanie flag
    int opt;
    while ((opt = getopt(argc, argv, "p:i:c:n:w:")) != -1) {
        switch (opt) {
            case 'p': particles_count = atoi(optarg); break;
            case 'i': iterations = atoi(optarg); break;
            case 'c': config_file = optarg; break;
            case 'n': log_interval = atoi(optarg); break;
            case 'w': 
                okn_x = atoi(optarg);
                // pobeanie drugiego argumentu dla flagi -w
                if (optind < argc && argv[optind][0] != '-') {
                    okn_y = atoi(argv[optind++]);
                }
                break;
            default:
                printf("Uzycie: %s <plik_mapy> [-p n] [-i n] [-c config] [-n n] [-w x y]\n", argv[0]);
                return 1;
        }
    }

    //sprawdzanie czy uzytkownik podal sciezke do pliku z mapa (argument pozycyjny po flagach)
    if (optind < argc) {
        m = argv[optind];
    } else {
        printf("Uzycie: %s <plik_mapy> [-p n] [-i n] [-c config] [-n n] [-w x y]\n", argv[0]);
        return 1;
    }

    // wczytywanie mapy
    double **mapa = load_map_from_file(m); 
    
    if (mapa == NULL) {
        return 1;
    }

    // domyslne parametry algorytmu: bezwladnosc, pamiec wlasna, wiedza roju
    PSOConfig config = {0.8, 0.9, 1.5};
    // Inicjalizacja biblioteki czcionek
    if (TTF_Init() == -1) {
        printf("Błąd SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }

    // Wczytanie czcionki (rozmiar 16 pikseli)
    // Upewnij się, że plik arial.ttf jest w tym samym folderze co kod!
    TTF_Font *font = TTF_OpenFont("ARIAL.TTF", 16);
    if (!font) {
        printf("Nie udalo sie wczytac czcionki: %s\n", TTF_GetError());
        // Możesz zdecydować, czy wyjść z błędem, czy kontynuować bez napisów
        return 1; 
    }
    // TWORZENIE MAPY!!
    int map_x = (int)mapa[0][0];
    int map_y = (int)mapa[0][1];
    SDL_Window *okno = SDL_CreateWindow("Mapa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, okn_x, okn_y, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(okno, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *map_tex = stworz_teksture_mapy(renderer, font, mapa, map_x, map_y, maks_z_mapy(mapa));
    
    if (config_file) load_config(config_file, &config);

    //inicjalizacja roju
    Swarm *swarm = init_swarm(particles_count, mapa);

    //inicjzalizacja loggera (tylko jesli uztkownik zdecyduje)
    FILE *log_fp = (log_interval > 0) ? init_logger("pso_log.csv") : NULL;

    //GLOWNA PETLA
    int running = 1;
    SDL_Event e;
    int t = 0;
    while(running && t < iterations){
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        t++;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); // czyszczenie rendera
        if (log_interval > 0 && (t % log_interval == 0)) { //zapis co log interval
            log_swarm(log_fp, t, swarm);
        }

        rysuj_mape(renderer, map_tex);
        rysuj_swarm(renderer, swarm, map_x, map_y, okn_x, okn_y);
        rysuj_statystyki(renderer, font, t, swarm->gBest_val);
        SDL_RenderPresent(renderer); //wyswietlanie utworzonej klatki
        
        update_swarm(swarm, mapa, config);
        SDL_Delay(30); //spowolnienie do 60fps

    }
    while (running){
            while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;}
            }
        SDL_Delay(500);
        
    }
    
    //wyswietlanie wyniku koncowego (STARE)
    //printf("X=%.2f Y=%.2f\n", swarm->gBest_x, swarm->gBest_y);
    //printf("Val=%.4f\n", swarm->gBest_val);

    //czyszczenie pamieci :)
    if (log_fp){
        close_logger(log_fp);
        printf("Plik z zapisem postępów zapisany do 'pso_log.csv'\n");
    }
    free_swarm(swarm);
    free_loaded_map(mapa);
    SDL_DestroyTexture(map_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(okno);
    SDL_Quit();

    return 0;
}