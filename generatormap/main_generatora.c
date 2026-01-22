#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "generator_map.h"

int main(int argc, char ** argv) {
    int opt;
    int debug_mode = 0;
    char *filename = "mapa.txt";

    while ((opt = getopt(argc, argv, "df:")) != -1) {
        switch (opt) {
            case 'd':
                debug_mode = 1;
                break;
            case 'f':
                filename = optarg;
                break;
            default:
                fprintf(stderr, "Użycie: %s [-d] [-f nazwa_pliku] <szerokość> <wysokość> <ilosc_sygnalow> <zasieg>\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (argc - optind != 4) {
        fprintf(stderr, "BŁĄD: Nieodpowiednia liczba argumentów!\nUżycie: %s [-d] [-f nazwa_pliku] <szerokość> <wysokość> <ilosc_sygnalow> <zasieg>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int width = atoi(argv[optind]);
    int height = atoi(argv[optind + 1]);
    int ILOSCSYGNALOW = atoi(argv[optind + 2]);
    int RADIUS = atoi(argv[optind + 3]);

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "BŁĄD: Wymiary mapy muszą być dodatnie! (Podano: %dx%d)\n", width, height);
        return EXIT_FAILURE;
    }

    if (debug_mode) printf("DEBUG: Inicjalizacja generatora liczb losowych.\n");
    srand(time(NULL));

    if (debug_mode) printf("DEBUG: Alokacja pamięci dla mapy %dx%d.\n", width, height);
    double **mapa = generate_empty_map(width, height);
    if (mapa == NULL) {
        fprintf(stderr, "BŁĄD: Brak pamięci dla mapy %dx%d!\n", width, height);
        return EXIT_FAILURE;
    }

    double curr = -100, maks = -100;
    if (debug_mode) printf("DEBUG: Rozpoczynanie dodawania %d sygnałów.\n", ILOSCSYGNALOW);

    for (int i = 0; i < ILOSCSYGNALOW - 1; i++) {
        curr = 15.0 + (double)rand() / RAND_MAX * 40.0;
        while (maks == curr) curr = 15.0 + (double)rand() / RAND_MAX * 40.0;
        if (maks < curr) maks = curr;
        
        if (debug_mode) {
            printf("DEBUG: Dodawanie sygnału %d: wartość %.2f, zasięg %d.\n", i + 1, curr, RADIUS);
        } else {
            internal_progress("Dodawanie sygnałów",i+2, ILOSCSYGNALOW);
        }
        
        map_add_signal(mapa, curr, RADIUS);
    }

    if (debug_mode) printf("DEBUG: Dodawanie sygnału końcowego: wartość 60.0, zasięg %d.\n", RADIUS);
    map_add_signal(mapa, 75.0, RADIUS*0.7);

    if (debug_mode) printf("DEBUG: Zapisywanie mapy do pliku '%s'.\n", filename);
    if (map_print_to_file(mapa, filename) != 0) {
        fprintf(stderr, "BŁĄD: Problem przy zapisie pliku!\n");
        free_map(mapa);
        return EXIT_FAILURE;
    }
    free_map(mapa);
    printf("Sukces: Mapa zapisana do '%s'.\n", filename);
    return EXIT_SUCCESS;
}