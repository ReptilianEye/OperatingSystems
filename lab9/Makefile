CC = gcc
CFLAGS = -Wall

all: life
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -lncursesw

life_threads: grid_threads.c life_threads.c
	$(CC) $(CFLAGS) grid_threads.c life_threads.c -o life_threads -lncursesw -lpthread
clean:
	rm -f life life_threads
