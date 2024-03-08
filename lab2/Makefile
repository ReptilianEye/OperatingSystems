GCC = gcc

main:
	${GCC} main.c -l libcollatz.o

compile_static:
	${GCC} -c libcollatz.c
	ar rcs libcollatz.a libcollatz.o
	${GCC} main.c -L. -l:libcollatz.a -o main


compile_shared:
	${GCC} -shared -fPIC libcollatz.c -o libcollatz.so

clean:
	rm -f *.o *.a *.so main