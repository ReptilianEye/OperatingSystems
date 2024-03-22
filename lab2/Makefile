GCC = gcc

# main:
# 	${GCC} main.c -l collatz.o

compile_static:
	${GCC} -c collatz.c -o libcollatz.o
	ar rcs libcollatz.a libcollatz.o
	${GCC} main.c -L. -l:libcollatz.a -o main -O0
	./main

compile_shared:
	${GCC} -fPIC -o collatz.o -c collatz.c 
	${GCC} -shared -o libcollatz.so collatz.o 
	${GCC} main.c -L. -l collatz -o main -O0
# export LD_LIBRARY_PATH=$PWD
	./main
	
compile_dynamic:
	make compile_shared
	${GCC} main.c -ldl -o main -O0 -Ddynamic 
	./main

# export LD_LIBRARY_PATH=$PWD
clean:
	rm -f *.o *.a *.so main