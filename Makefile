CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = pso
SRCS = main.c map.c pso.c utils.c
OBJS = $(SRCS:.c=.o)
GENE = generatormap/generator_map.c generatormap/generator_map.h generatormap/main_generatora.c


all: main.c map_loader.h map_loader.c
	$(CC) $(CFLAGS) main.c map_loader.c map_loader.h -o pso

generator: $(GENE)
	$(CC) $(CFLAGS) $(GENE) -o generator -lm
