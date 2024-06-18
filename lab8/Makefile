CC = gcc

make printers: printers.c
	$(CC) -o printers printers.c -lrt -lpthread
	./printers 2

make users: users.c
	$(CC) -o users users.c -lrt -lpthread
	./users 5
	

make clean:
	find . -type f -executable -delete
	find . -type p -delete