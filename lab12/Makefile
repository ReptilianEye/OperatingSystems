CC=gcc

BUILD_DIR=build

CFLAGS := -g -O0 -std=c11 -Wall 

.PHONY: all clear run_client run_server

all: server client

socketutil: socketutil.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/$@ -c $<
	ar rcs build/lib$@.a build/$@

client: SocketClient.c socketutil
	mkdir -p $(BUILD_DIR)
	make socketutil
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $< build/libsocketutil.a -lpthread

server: ServerUDP.c socketutil
	mkdir -p $(BUILD_DIR)
	make socketutil
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $< build/libsocketutil.a -lpthread

run_client: client 
	./$(BUILD_DIR)/client

run_server: server
	./$(BUILD_DIR)/server
	
clean:
	rm -rf $(BUILD_DIR)