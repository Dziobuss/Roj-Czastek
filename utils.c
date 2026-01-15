#include <stdlib.h>
#include "utils.h"

double random_double(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}