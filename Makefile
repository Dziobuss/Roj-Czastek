CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = pso
SRCS = main.c map.c pso.c utils.c
OBJS = $(SRCS:.c=.o)
GENE = generatormap/generator_map.c generatormap/generator_map.h generatormap/main_generatora.c
all: $(TARGET)

generator: $(GENE)
	$(CC) $(CFLAGS) $(GENE) -o generator -lm
