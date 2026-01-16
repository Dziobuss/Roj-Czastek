#ifndef GRAFIKA_H
#define GRAFIKA_H
#include <SDL2/SDL.h>
// Funkcja pobiera renderer, wskaźnik do tablicy double** oraz wymiary okna
void wyswietl_mape(SDL_Renderer *renderer, double **mapa, int win_w, int win_h, double maks_signal);
#endif