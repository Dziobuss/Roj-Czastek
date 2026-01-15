#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "map_loader.h"
#include "pso.h"
#include "logger.h"
#include "utils.h"

void load_config(const char *filename, PSOConfig *cfg) {
    FILE *f = fopen(filename, "r");
    if (f) {
        fscanf(f, "%lf %lf %lf", &cfg->w, &cfg->c1, &cfg->c2);
        fclose(f);
    }
}

int main(int argc, char **argv){
    srand(time(NULL));

    if (argc < 2) {
        printf("Uzycie: %s <plik_mapy> [-p n] [-i n] ...\n", argv[0]);
        return 1;
    }

    char *m = argv[1];
    double **mapa = load_map_from_file(m); 
    
    if (mapa == NULL) {
        return 1;
    }
    
    int particles_count = 30;
    int iterations = 100;
    int log_interval = 0;
    char *config_file = NULL;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) particles_count = atoi(argv[++i]);
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) iterations = atoi(argv[++i]);
        else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) config_file = argv[++i];
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) log_interval = atoi(argv[++i]);
    }

    PSOConfig config = {0.5, 1.0, 1.0};
    if (config_file) load_config(config_file, &config);

    Swarm *swarm = init_swarm(particles_count, mapa);
    FILE *log_fp = (log_interval > 0) ? init_logger("pso_log.csv") : NULL;

    for (int t = 0; t < iterations; t++) {
        update_swarm(swarm, mapa, config);

        if (log_interval > 0 && (t % log_interval == 0)) {
            log_swarm(log_fp, t, swarm);
        }
    }

    printf("X=%.2f Y=%.2f\n", swarm->gBest_x, swarm->gBest_y);
    printf("Val=%.4f\n", swarm->gBest_val);

    if (log_fp) close_logger(log_fp);
    free_swarm(swarm);
    free_loaded_map(mapa);

    return 0;
}