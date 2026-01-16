#include <stdlib.h>
#include <stdio.h>


double **load_map_from_file(char *m){
    FILE *f = fopen(m, "r");
    if (f == NULL) return NULL;
    int width, height;
    if (fscanf(f, "%d %d", &width, &height) != 2) {
        fclose(f);
        return NULL;
    }
    double **mapa = malloc(sizeof(double*)*(height+1));
    for (int i = 0;i<=height;i++){
        mapa[i] = malloc(sizeof(double)*width);
    }
    double liczba;
    int j = 1;
    int i = 0;
    mapa[0][0] = width;
    mapa[0][1] = height;
    while(fscanf(f,"%lf", &liczba)!=EOF){
        mapa[j][i] = liczba;
        i+=1;
        if (i == width){
            j += 1;
            i = 0;
        }
    }
    fclose(f);
    return mapa;
}
double maks_z_mapy(double **mapa){ //potrzebne do generacji wizualnej
    int width = mapa[0][0];
    int height = mapa[0][1];
    double maks = -100.;
    for (int i =1;i<=height;i++){
        for(int j = 0;j<width;j++){
            if(mapa[i][j]>maks){
                maks = mapa[i][j];
            }
        }
    }
    return maks;
}
// przenioslem tu zeby byl porzadek
void free_loaded_map(double **map) {
    if (!map) return;
    int height = (int)map[0][1];
    for (int i = 0; i <= height; i++) {
        free(map[i]);
    }
    free(map);
}