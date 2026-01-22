#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "generator_map.h"

void internal_progress(const char *task, int current, int total) {
    int width = 30;
    float progress = (float)current / total;
    int filled = progress * width;
    printf("\r%s: [", task);
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("#");
        else printf(" ");
    }
    printf("] %d%%", (int)(progress * 100));
    fflush(stdout);
    if (current == total) printf("\n");
}

double ** generate_empty_map(int width, int height){
    double **mapa = malloc((height + 1) * sizeof(double *));
    mapa[0] = (double*)malloc(2 * sizeof(int));
    ((int*)mapa[0])[0] = width;
    ((int*)mapa[0])[1] = height;
    
    for(int i = 1; i <= height; i++){
        mapa[i] = malloc(width * sizeof(double));
        for(int j = 0; j < width; j++){
            mapa[i][j] = -100.0;
        }
        if (i % (height / 10 + 1) == 0 || i == height) {
            internal_progress("Alokacja", i, height);
        }
    }
    return mapa;
}

void map_add_signal(double **mapa, double signal, int radius) {
    int width = ((int*)mapa[0])[0];
    int height = ((int*)mapa[0])[1];
    int centerX = rand() % width;
    int centerY = rand() % height;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int currentX = centerX + dx;
            int currentY = centerY + dy;
            if (currentX >= 0 && currentX < width && currentY >= 0 && currentY < height) {
                double distance = sqrt(dx*dx + dy*dy);
                if (distance <= radius) {
                    double attenuation = 1.0 - (distance / (double)radius);
                    double fluctuation = 0.7 + ((rand() % 31) / 100.0); 
                    double final_signal = signal * attenuation * fluctuation;
                    mapa[currentY + 1][currentX] = (final_signal > mapa[currentY+1][currentX]) ? final_signal : mapa[currentY+1][currentX];
                }
            }
        }
    }
}

int map_print_to_file(double **mapa, const char *plik){
    FILE *f = fopen(plik, "w");
    if (f == NULL){
        return -1;
    }
    int height = ((int*)mapa[0])[1];
    int width = ((int*)mapa[0])[0];
    fprintf(f,"%d %d\n",width, height);
    
    for (int i = 1; i < height + 1; i++){
        for(int j = 0; j < width; j++){
            fprintf(f,"%g ", mapa[i][j]);
        }
        fprintf(f,"\n");
        if (i % (height / 10 + 1) == 0 || i == height) {
            internal_progress("Zapis", i, height);
        }
    }
    fclose(f);
    return 0;
}

void free_map(double **mapa){
    int height = ((int*)mapa[0])[1];
    for(int i = 0; i <= height; i++){
        free(mapa[i]);
        if (i % (height / 10 + 1) == 0 || i == height) {
            internal_progress("Czyszczenie", i, height);
        }
    }
    free(mapa);
}