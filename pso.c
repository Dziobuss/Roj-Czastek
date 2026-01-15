#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pso.h"
#include "utils.h"

double get_map_value_internal(double **map, double x, double y) {
    int width = (int)map[0][0];
    int height = (int)map[0][1];

    int col = (int)x;
    int row = (int)y;

    if (col < 0 || col >= width || row < 0 || row >= height) {
        return -10000.0;
    }

    return map[row + 1][col]; 
}

Swarm* init_swarm(int count, double **map) {
    Swarm *swarm = (Swarm*)malloc(sizeof(Swarm));
    swarm->particle_count = count;
    swarm->particles = (Particle*)malloc(count * sizeof(Particle));
    swarm->gBest_val = -1e9;

    int width = (int)map[0][0];
    int height = (int)map[0][1];

    for (int i = 0; i < count; i++) {
        swarm->particles[i].x = random_double(0, width);
        swarm->particles[i].y = random_double(0, height);
        swarm->particles[i].vx = 0;
        swarm->particles[i].vy = 0;

        double val = get_map_value_internal(map, swarm->particles[i].x, swarm->particles[i].y);
        
        swarm->particles[i].current_val = val;
        swarm->particles[i].pBest_val = val;
        swarm->particles[i].pBest_x = swarm->particles[i].x;
        swarm->particles[i].pBest_y = swarm->particles[i].y;

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

        double r1 = random_double(0, 1);
        double r2 = random_double(0, 1);

        p->vx = config.w * p->vx 
              + config.c1 * r1 * (p->pBest_x - p->x) 
              + config.c2 * r2 * (swarm->gBest_x - p->x);
        
        p->vy = config.w * p->vy 
              + config.c1 * r1 * (p->pBest_y - p->y) 
              + config.c2 * r2 * (swarm->gBest_y - p->y);

        p->x += p->vx;
        p->y += p->vy;

        double current_fitness = get_map_value_internal(map, p->x, p->y);
        p->current_val = current_fitness;

        if (current_fitness > p->pBest_val) {
            p->pBest_val = current_fitness;
            p->pBest_x = p->x;
            p->pBest_y = p->y;
        }

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

void free_loaded_map(double **map) {
    if (!map) return;
    int height = (int)map[0][1];
    for (int i = 0; i <= height; i++) {
        free(map[i]);
    }
    free(map);
}