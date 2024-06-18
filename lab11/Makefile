CC = gcc
CFLAGS = -Wall

	
server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

run_server: server
	./server 127.0.0.1 8000

run_client: client
	./client 127.0.0.1 8000

clean:
	rm -f server client	
