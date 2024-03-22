CC = gcc


make spawner:
	$(CC) -o spawner spawner.c
	./spawner 10

make sender:
	$(CC) -o sender sender.c
	./sender .

make clean:
	find . -type f -executable -delete
