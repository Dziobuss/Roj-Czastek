#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include "pso.h"

FILE* init_logger(const char *filename);
void log_swarm(FILE *fp, int iteration, Swarm *swarm);
void close_logger(FILE *fp);

#endif