#include <stdlib.h>
#include <stdio.h>
#include "map_loader.h"
int main(int argc, char ** argv){
    char *m = argv[1];
    double **mapa = load_map_from_file(m); //wczytywanie mapki poki co

}