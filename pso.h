#ifndef PSO_H
#define PSO_H

typedef struct {
    double w; // bezwladnosc
    double c1; // zaufanie do wlasnej pamieci
    double c2; // zaufanie do danych roju
} PSOConfig;

//struktura pojedynczego drona
typedef struct {
    double x, y; // aktualne wsporzedne na mapie
    double vx, vy; // wektor predkosci
    double pBest_x, pBest_y; // zapamietana pozycja najlepszego sygnalu
    double pBest_val; // sygnal w najlepszym punkcie pBest
    double current_val; // sila sygnalu w obecnym polozeniu
} Particle;

//struktura calego roju
typedef struct {
    Particle *particles; // tablica przechowujaca drony
    int particle_count; //ilosc dronow w roju
    double gBest_x, gBest_y; //wspolrzedne najlepszego punktu znalezionego przez roj
    double gBest_val; // wartosc sygnalu w punkcie gBest
} Swarm;

//deklaracje uzywanych funkcji
Swarm* init_swarm(int count, double **map);
void update_swarm(Swarm *swarm, double **map, PSOConfig config);
void free_swarm(Swarm *swarm);
 //void free_loaded_map(double **map); w maploaderze

#endif