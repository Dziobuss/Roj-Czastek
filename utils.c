#include <stdlib.h>
#include "utils.h"

// zwracanie double z podanego zakresu
double random_double(double min, double max) {
    // {przesuniecie} + {wartosc z zakresu 0.0 1.0} * {zakres w ktorym operujemy - czyli dostosowanie wartosci do zakresu min max} 
    return min + ((double)rand() / RAND_MAX) * (max - min);
}