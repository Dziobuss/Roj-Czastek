#ifndef PSO_H
#define PSO_H

typedef struct {
    double w;
    double c1;
    double c2;
} PSOConfig;

typedef struct {
    double x, y;
    double vx, vy;
    double pBest_x, pBest_y;
    double pBest_val;
    double current_val;
} Particle;

typedef struct {
    Particle *particles;
    int particle_count;
    double gBest_x, gBest_y;
    double gBest_val;
} Swarm;

Swarm* init_swarm(int count, double **map);
void update_swarm(Swarm *swarm, double **map, PSOConfig config);
void free_swarm(Swarm *swarm);
void free_loaded_map(double **map);

#endif