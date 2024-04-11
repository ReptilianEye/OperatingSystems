CC = gcc

make pipe:
	$(CC) -o pipe pipe.c
	./pipe 0.01 100

make manager:
	$(CC) -o manager integral_manager.c
	./manager 0 1 100
	

make slave:
	$(CC) -o slave integral_slave.c
	./slave

make clean:
	find . -type f -executable -delete
	find . -type p -delete
