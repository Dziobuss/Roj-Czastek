#include "grafika.h"
#include <math.h>
#include <SDL2/SDL_ttf.h>

// Pomocnicza funkcja do rysowania tekstu bezpośrednio na teksturze/targetcie
static void narysuj_tekst_prosty(SDL_Renderer *renderer, TTF_Font *font, const char *tekst, int x, int y) {
    if (!font) return;
    SDL_Color czarny = {0, 0, 0, 255}; 
    SDL_Surface *surf = TTF_RenderText_Blended(font, tekst, czarny);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_RenderCopy(renderer, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

static void rysuj_kolo(SDL_Renderer *renderer, int x, int y, int promien) {
    for (int dy = -promien; dy <= promien; dy++) {
        int dx = (int)sqrt(promien * promien - dy * dy);
        SDL_RenderDrawLine(renderer, x - dx, y + dy, x + dx, y + dy);
    }
}

static void rysuj_trojkat(SDL_Renderer *renderer, int x, int y, int rozmiar) {
    SDL_Vertex vertices[3];
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    vertices[0].position = (SDL_FPoint){ (float)x, (float)(y - rozmiar) };
    vertices[0].color = (SDL_Color){ r, g, b, a };
    vertices[1].position = (SDL_FPoint){ (float)(x - rozmiar), (float)(y + rozmiar) };
    vertices[1].color = (SDL_Color){ r, g, b, a };
    vertices[2].position = (SDL_FPoint){ (float)(x + rozmiar), (float)(y + rozmiar) };
    vertices[2].color = (SDL_Color){ r, g, b, a };

    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

static void pusty_trojkat(SDL_Renderer *renderer, int x, int y, int rozmiar) {
    SDL_Point punkty[4];
    punkty[0].x = x; punkty[0].y = y - rozmiar;
    punkty[1].x = x - rozmiar; punkty[1].y = y + rozmiar;
    punkty[2].x = x + rozmiar; punkty[2].y = y + rozmiar;
    punkty[3] = punkty[0];
    SDL_RenderDrawLines(renderer, punkty, 4);
}

SDL_Texture* stworz_teksture_mapy(SDL_Renderer *renderer, TTF_Font *font, double **mapa, int map_w, int map_h, double maks_signal) {
    // 1. Tworzymy teksturę w rozmiarze mapy
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
            if (fabs(wartosc - maks_signal) < 0.0001) {
                max_i = i; max_j = j;
            }
            double t = wartosc / maks_signal; 
            if (t > 1.0) t = 1.0;
            Uint8 g = (Uint8)(255 * t);
            Uint8 b = (Uint8)(255 * (1.0 - t));
            SDL_SetRenderDrawColor(renderer, 0, g, b, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }

    // 3. Rysujemy trójkąt celu (maksimum) na mapie
    int rozmiar_trojkata = map_w / 70;
    if (rozmiar_trojkata < 3) rozmiar_trojkata = 3;
    if (max_i != -1) {
        SDL_SetRenderDrawColor(renderer, 222, 87, 2, 255);
        rysuj_trojkat(renderer, max_i, max_j, rozmiar_trojkata);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        pusty_trojkat(renderer, max_i, max_j, rozmiar_trojkata);
    }

    // --- DODAWANIE LEGENDY NA TEKSTURĘ ---
    // Pozycjonujemy w prawym górnym rogu tekstury
    int leg_w = 210;
    int leg_h = 80;
    SDL_Rect leg_rect = { map_w - leg_w - 10, 10, leg_w, leg_h };
    
    // Beżowe tło
    SDL_SetRenderDrawColor(renderer, 245, 245, 220, 255);
    SDL_RenderFillRect(renderer, &leg_rect);
    // Czarna obwódka
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &leg_rect);

    // Ikona cząstki (Białe koło)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    rysuj_kolo(renderer, map_w - leg_w + 10, 30, 6);
    narysuj_tekst_prosty(renderer, font, "Czastki", map_w - leg_w + 30, 20);

    // Ikona celu (Trójkąt)
    SDL_SetRenderDrawColor(renderer, 222, 87, 2, 255);
    rysuj_trojkat(renderer, map_w - leg_w + 10, 65, 8);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    pusty_trojkat(renderer, map_w - leg_w + 10, 65, 8);
    narysuj_tekst_prosty(renderer, font, "Najsilniejszy sygnal", map_w - leg_w + 30, 55);

    SDL_SetRenderTarget(renderer, NULL);
    return tekstura;
}

void rysuj_mape(SDL_Renderer *renderer, SDL_Texture *tekstura_mapy) {
    SDL_RenderCopy(renderer, tekstura_mapy, NULL, NULL);
}

void rysuj_swarm(SDL_Renderer *renderer, Swarm *swarm, int map_x, int map_y, int okn_x, int okn_y) {
    if (swarm == NULL || swarm->particles == NULL) return;
    float scale_x = (float)okn_x / map_x;
    float scale_y = (float)okn_y / map_y;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < swarm->particle_count; i++) {
        Particle p = swarm->particles[i];
        rysuj_kolo(renderer, (int)(p.x * scale_x), (int)(p.y * scale_y), 6);
    }
}
void rysuj_statystyki(SDL_Renderer *renderer, TTF_Font *font, int iteracja, double best_val) {
    if (!font) return;

    char buf[128];
    sprintf(buf, "Iteracja: %d | Najlepszy znaleziony sygnal: %.4f", iteracja, best_val);

    SDL_Color bialy = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(font, buf, bialy);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    // Wyświetlamy w lewym górnym rogu (x=10, y=10)
    SDL_Rect dst = { 10, 10, surf->w, surf->h };
    
    // Opcjonalnie: mały czarny prostokąt pod tekstem, żeby był bardziej czytelny
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // Półprzezroczysty czarny
    SDL_Rect bg = { 5, 5, surf->w + 10, surf->h + 10 };
    SDL_RenderFillRect(renderer, &bg);

    SDL_RenderCopy(renderer, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}