#include "grafika.h"

void wyswietl_mape(SDL_Renderer *renderer, double **mapa, int okn_x, int okn_y, double maks_signal) {
    if (mapa == NULL) return;
    int width = (int)mapa[0][0];
    int height = (int)mapa[0][1]; //pobranie wymiarow
    float tile_w = (float)okn_x / width; //wymiar 1 kafelka na pikselach
    float tile_h = (float)okn_y / height;

    // Zakresy dla gradientu
    double min_signal = -100.0;
    double max_signal = maks_signal; // Przyjmujemy maksa z mapy

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            double wartosc = (mapa[j + 1][i]<0)? 0 : mapa[j+1][i];

            // normalizujemy do zakresu 0.0 - 1.0
            double t = (wartosc - min_signal) / (max_signal - min_signal);
            if (t < 0.0) t = 0.0;
            if (t > 1.0) t = 1.0;

            // Obliczanie koloru na podstawie wspolczynnika t
            Uint8 r = (Uint8)(120 * (1.0 - t));
            Uint8 g = (Uint8)(120*t);
            Uint8 b = (Uint8)(120 * t);

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);

            // Tworzenie prostokąta dla danego punktu sygnału
            SDL_FRect rect;
            rect.x = i * tile_w;
            rect.y = j * tile_h;
            rect.w = tile_w;
            rect.h = tile_h;

            // Rysowanie wypełnionego prostokąta
            SDL_RenderFillRectF(renderer, &rect);
        }
    }
}