#include "logger.h"

FILE* init_logger(const char *filename) {
    // zapisywanie do pliku
    FILE *fp = fopen(filename, "w");
    if (fp) {
        // naglowek CSV 
        fprintf(fp, "iter,id,x,y,fitness\n");
    }
    return fp;
}

void log_swarm(FILE *fp, int iteration, Swarm *swarm) {
    if (!fp) return;
    // zapisywanie stanu czastek do pliku
    for (int i = 0; i < swarm->particle_count; i++) {
        fprintf(fp, "%d,%d,%.4f,%.4f,%.4f\n", 
                iteration, 
                i, 
                swarm->particles[i].x, 
                swarm->particles[i].y, 
                swarm->particles[i].current_val);
    }
}

void close_logger(FILE *fp) {
    if (fp) fclose(fp);
}