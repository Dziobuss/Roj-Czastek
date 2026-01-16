#ifndef MAPLOADER
#define MAPLOADER
double **load_map_from_file(char *m); //wczytuje mape
double maks_z_mapy(double **mapa); //zwraca x, y i value maksa na mapie
void free_loaded_map(double **map); //zwalnia mape
#endif