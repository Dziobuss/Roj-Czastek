#ifndef GENERATORMAP
#define GENERATORMAP
int map_print_to_file(double **mapa, const char *plik); //printuje mape do pliku
double ** generate_empty_map(int width,int height); //generuje mape wypełnioną -100 w poprawnym formacie
void map_add_signal(double **mapa, double signal, int radius); //dodaje w losowym miejscu na mapie sygnal o podanej wartosci
void free_map(double **mapa); //uwalnia mape z pamieci
#endif