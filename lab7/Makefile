CC = gcc

make server: server.c
	$(CC) -o server server.c

make client: client.c
	$(CC) -o client client.c

make clean:
	find . -type f -executable -delete
	find . -type p -delete