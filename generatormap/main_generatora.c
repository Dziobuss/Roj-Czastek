#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "generator_map.h"
#define ILOSCSYGNALOW 10
#define RADIUS 3
int main(int argc, char ** argv) {
    if (argc != 3) {
        fprintf(stderr, "BŁĄD: Nieodpowiednia liczba argumentów!\nUżycie: %s <szerokość> <wysokość>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "BŁĄD: Wymiary mapy muszą być dodatnie! (Podano: %dx%d)\n", width, height);
        return EXIT_FAILURE;
    }
    srand(time(NULL));
    double **mapa = generate_empty_map(width, height);
    if (mapa == NULL) {
        fprintf(stderr, "BŁĄD: Brak pamięci dla mapy %dx%d!\n", width, height);
        return EXIT_FAILURE;
    }
    double curr = -100, maks = -100;
    for (int i = 0; i < ILOSCSYGNALOW; i++) {
        curr = 15.0 + (double)rand() / RAND_MAX * 40.0;
        while (maks == curr) curr = 15.0 + (double)rand() / RAND_MAX * 40.0;
        if (maks < curr) maks = curr;
        map_add_signal(mapa, curr, RADIUS);
    }
    if (map_print_to_file(mapa, "mapa.txt") == 0) {
        printf("Sukces: Mapa zapisana do 'mapa.txt'.\n");
    } else {
        fprintf(stderr, "BŁĄD: Problem przy zapisie pliku!\n");
        free_map(mapa);
        return EXIT_FAILURE;
    }
    free_map(mapa);
    return EXIT_SUCCESS;
}