#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pso.h"
#include "utils.h"

// pobieranie wartosci z mapy
double get_map_value_internal(double **map, double x, double y) {
    // odczytywanie wymiarow
    int width = (int)map[0][0];
    int height = (int)map[0][1];

    // rzutowanie pozycji na tablice
    int col = (int)x;
    int row = (int)y;

    // zabezpieczenia przed wyjsciem poza tablice
    if (col < 0 || col >= width || row < 0 || row >= height) {
        return -10000.0;
        // drastycznie mala wartosc, aby algorytm zawrocil drona
    }

    //zaczynamy od wiersza 1 zgodnie ze struktura zapisana w map_loader.c
    return map[row + 1][col]; 
}

Swarm* init_swarm(int count, double **map) {
    Swarm *swarm = (Swarm*)malloc(sizeof(Swarm));
    swarm->particle_count = count;
    swarm->particles = (Particle*)malloc(count * sizeof(Particle));
    
    // inicjalizacja najlepszej wartosci roju
    // mala wartosc startowa aby nawet najslabszy znaleziony punkt byl lepszy od tej
    swarm->gBest_val = -1e9;
    //odczytywanie wymiarow
    int width = (int)map[0][0];
    int height = (int)map[0][1];

    for (int i = 0; i < count; i++) {
        // losowanie pozycji startowej w granicach mapy
        swarm->particles[i].x = random_double(0, width);
        swarm->particles[i].y = random_double(0, height);
        
        //nadanie predkosci poczatkowej = 0
        swarm->particles[i].vx = 0;
        swarm->particles[i].vy = 0;

        // sprawdzanie mocy sygnalu w poczatkowym polozeniu dronow
        double val = get_map_value_internal(map, swarm->particles[i].x, swarm->particles[i].y);
        
        // na poczatku najlepsza znana pozycja to ta poczatkowa
        swarm->particles[i].pBest_val = val;
        swarm->particles[i].pBest_x = swarm->particles[i].x;
        swarm->particles[i].pBest_y = swarm->particles[i].y;

        // jesli obecny dron ma lepszy sygnal niz poprzedni najelpszy -> obecny staje sie najlepszy
        if (val > swarm->gBest_val) {
            swarm->gBest_val = val;
            swarm->gBest_x = swarm->particles[i].x;
            swarm->gBest_y = swarm->particles[i].y;
        }
    }
    return swarm;
}

void update_swarm(Swarm *swarm, double **map, PSOConfig config) {
    for (int i = 0; i < swarm->particle_count; i++) {
        Particle *p = &swarm->particles[i];

        //wprowadznie wspolczynnikow losowych z zakresu 0-1 w celu wprowadzenia elementu stochastycznego - ruch byl bardziej naturalny
        double r1 = random_double(0, 1);
        double r2 = random_double(0, 1);

        //wzor na predkosc:
        // 1. bezwladnosc: (w * v) - lec tam gdzie leciales do tej pory
        // 2. pamiec (c1 * r1 * ...) - lec w strone swojego najlepszego wyniku
        // 3. Socjal (c2 * r2 * ...) - lec w strone globalnego najlepszego wyniku gBest
        p->vx = config.w * p->vx 
              + config.c1 * r1 * (p->pBest_x - p->x) 
              + config.c2 * r2 * (swarm->gBest_x - p->x);
        
        p->vy = config.w * p->vy 
              + config.c1 * r1 * (p->pBest_y - p->y) 
              + config.c2 * r2 * (swarm->gBest_y - p->y);

        // aktualizowanie aktualnej pozycji
        p->x += p->vx*0.2;
        p->y += p->vy*0.2;

        // ocena zaaktualizowanego miejsca
        double current_fitness = get_map_value_internal(map, p->x, p->y);
        p->current_val = current_fitness;

        // Aaktualizacja osobisteej najlepszej wartosci pBest
        if (current_fitness > p->pBest_val) {
            p->pBest_val = current_fitness;
            p->pBest_x = p->x;
            p->pBest_y = p->y;
        }

        // aktualizacja globalnej najlepszej wartosci gBest
        if (current_fitness > swarm->gBest_val) {
            swarm->gBest_val = current_fitness;
            swarm->gBest_x = p->x;
            swarm->gBest_y = p->y;
        }
    }
}

void free_swarm(Swarm *swarm) {
    if (swarm) {
        free(swarm->particles);
        free(swarm);
    }
}
