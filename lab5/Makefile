CC = gcc

make demo:
	$(CC) -o demo signals_demo.c
# ./demo none
# ./demo ignore
# ./demo handler
# ./demo mask

make cacher:
	$(CC) -o catcher catcher.c
	./catcher
	

make sender ARGS="arg1":
	$(CC) -o sender sender.c
	./sender $(ARGS) 1
	./sender $(ARGS) 2
	./sender $(ARGS) 1
	./sender $(ARGS) 2
	./sender $(ARGS) 3


make clean:
	find . -type f -executable -delete
