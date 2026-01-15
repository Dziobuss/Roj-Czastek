CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = pso
SRCS = main.c map_loader.c pso.c utils.c Logger.c
GENE = generatormap/generator_map.c generatormap/generator_map.h generatormap/main_generatora.c

all: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lm

generator: $(GENE)
	$(CC) $(CFLAGS) $(GENE) -o generator -lm

clean:
	rm -f $(TARGET) generator pso_log.csv