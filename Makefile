CC = gcc
CFLAGS = -Wall -Wextra -g -lm

TARGET = pso
SRCS = main.c map.c pso.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

main.o: main.c map.h pso.h 
	$(CC) -c main.c $(CFLAGS)

map.o: map.c map.h
	$(CC) -c map.c $(CFLAGS)

pso.o: pso.c pso.h map.h
	$(CC) -c pso.c $(CFLAGS)
clean:
	rm -f $(OBJS) $(TARGET) pso_log.csv
