#include "grafika.h"
#include <math.h>
static void rysuj_kolo(SDL_Renderer *renderer, int x, int y, int promien) {
    for (int dy = -promien; dy <= promien; dy++) {
        // Obliczamy szerokość linii na danej wysokości dy
        int dx = (int)sqrt(promien * promien - dy * dy);
        // Rysujemy linię od lewej do prawej krawędzi
        SDL_RenderDrawLine(renderer, x - dx, y + dy, x + dx, y + dy);
    }
}
static void rysuj_trojkat(SDL_Renderer *renderer, int x, int y, int rozmiar) {
    SDL_Vertex vertices[3];
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    // Wierzchołek górny
    vertices[0].position = (SDL_FPoint){ (float)x, (float)(y - rozmiar) };
    vertices[0].color = (SDL_Color){ r, g, b, a };

    // Wierzchołek dolny lewy
    vertices[1].position = (SDL_FPoint){ (float)(x - rozmiar), (float)(y + rozmiar) };
    vertices[1].color = (SDL_Color){ r, g, b, a };

    // Wierzchołek dolny prawy
    vertices[2].position = (SDL_FPoint){ (float)(x + rozmiar), (float)(y + rozmiar) };
    vertices[2].color = (SDL_Color){ r, g, b, a };

    // Rysujemy trójkąt z tych 3 punktów (brak indeksów = rysuj wprost)
    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}
static void pusty_trojkat(SDL_Renderer *renderer, int x, int y, int rozmiar) {
    // Definiujemy 4 punkty (3 wierzchołki + powrót do początku)
    SDL_Point punkty[4];

    // Wierzchołek górny
    punkty[0].x = x;
    punkty[0].y = y - rozmiar;

    // Wierzchołek dolny lewy
    punkty[1].x = x - rozmiar;
    punkty[1].y = y + rozmiar;

    // Wierzchołek dolny prawy
    punkty[2].x = x + rozmiar;
    punkty[2].y = y + rozmiar;

    // Zamknięcie trójkąta (powrót do góry)
    punkty[3] = punkty[0];

    // Rysowanie obwódki
    SDL_RenderDrawLines(renderer, punkty, 4);
}
SDL_Texture* stworz_teksture_mapy(SDL_Renderer *renderer, double **mapa, int map_w, int map_h, double maks_signal) {
    // 1. Tworzymy teksturę w rozmiarze mapy (np. 100x100 pikseli)
    SDL_Texture *tekstura = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, map_w, map_h);
    if (!tekstura) return NULL;

    SDL_SetRenderTarget(renderer, tekstura);
    
    int max_i = -1, max_j = -1;

    // 2. Rysujemy kolory sygnału piksel po pikselu
    for (int j = 0; j < map_h; j++) {
        for (int i = 0; i < map_w; i++) {
            double wartosc = mapa[j + 1][i];
            if (wartosc < 0) wartosc = 0;

            // Szukamy pozycji maxa do późniejszego narysowania trójkąta
            if (fabs(wartosc - maks_signal) < 0.0001) {
                max_i = i;
                max_j = j;
            }

            double t = wartosc / maks_signal; 
            if (t > 1.0) t = 1.0;

            Uint8 g = (Uint8)(255 * t);
            Uint8 b = (Uint8)(255 * (1.0 - t));

            SDL_SetRenderDrawColor(renderer, 0, g, b, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }

    // 3. Rysujemy trójkąt bezpośrednio NA TEKSTURZE
    if (max_i != -1) {
        int rozmiar_trojkata = map_w / 40; // Skalujemy rozmiar trójkąta do wielkości mapy
        if (rozmiar_trojkata < 3) rozmiar_trojkata = 3;

        SDL_SetRenderDrawColor(renderer, 222, 87, 2, 255); // Pomarańczowy
        rysuj_trojkat(renderer, max_i, max_j, rozmiar_trojkata);
        
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Czerwona obwódka
        pusty_trojkat(renderer, max_i, max_j, rozmiar_trojkata);
    }

    SDL_SetRenderTarget(renderer, NULL);
    return tekstura;
}
void rysuj_mape(SDL_Renderer *renderer, SDL_Texture *tekstura_mapy) {
    // poki co null null pozniej sie jeszcze to zmieni
    SDL_RenderCopy(renderer, tekstura_mapy, NULL, NULL);
}
void rysuj_swarm(SDL_Renderer *renderer, Swarm *swarm, int map_x, int map_y, int okn_x, int okn_y) {
    if (swarm == NULL || swarm->particles == NULL) return;

    // Przelicznik: ile pikseli przypada na jedną jednostkę mapy
    float scale_x = (float)okn_x / map_x;
    float scale_y = (float)okn_y / map_y;

    // 1. Rysujemy wszystkie drony (cząsteczki)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Biały kolor dla dronów

    for (int i = 0; i < swarm->particle_count; i++) {
        Particle p = swarm->particles[i];
        rysuj_kolo(renderer, (float)(p.x * scale_x), (float)(p.y * scale_y), 6);
        
    }
}