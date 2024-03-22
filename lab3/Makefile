CC = gcc


make revBy1:
	$(CC) -o revBy1 revBy1.c
	./revBy1 in.txt temp

make revBy1024:
	$(CC) -o revBy1024 revBy1024.c
	./revBy1024 in.txt temp

make fileSizes:
	$(CC) -o fileSizes fileSizes.c
	./fileSizes

make clean:
	find . -type f -executable -delete
	rm -f temp
