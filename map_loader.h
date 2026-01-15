#ifndef MAPLOADER
#define MAPLOADER
double **load_map_from_file(char *m); //wczytuje mape
void free_loaded_map(double **map); //zwalnia mape
#endif