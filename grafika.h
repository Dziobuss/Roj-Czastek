#ifndef GRAFIKA_H
#define GRAFIKA_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "pso.h"
// Funkcja pobiera renderer, wskaźnik do tablicy double** oraz wymiary okna
SDL_Texture* stworz_teksture_mapy(SDL_Renderer *renderer, TTF_Font *font, double **mapa, int map_w, int map_h, double maks_signal);
void rysuj_mape(SDL_Renderer *renderer, SDL_Texture *tekstura_mapy);
// nakłada wszystkie particle ze swarmu na mapę
void rysuj_swarm(SDL_Renderer *renderer, Swarm *swarm, int map_x, int map_y, int okn_x, int okn_y);
void rysuj_statystyki(SDL_Renderer *renderer, TTF_Font *font, int iteracja, double best_val);
#endif