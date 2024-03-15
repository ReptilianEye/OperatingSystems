GCC = gcc

# main:
# 	${GCC} main.c -l collatz.o

compile_static:
	${GCC} -c collatz.c -o libcollatz.o
	ar rcs libcollatz.a libcollatz.o
	${GCC} main.c -L. -l:libcollatz.a -o main
# ./main

compile_shared:
	${GCC} -fPIC -o collatz.o -c collatz.c 
	${GCC} -shared -o libcollatz.so collatz.o 
	${GCC} main.c -L. -l collatz -o main
	export LD_LIBRARY_PATH=$PWD
# ./main

compile_dynamic:
	make compile_shared
	${GCC} main_dyn.c -ldl -o main_dyn
	export LD_LIBRARY_PATH=$PWD
# ./main_dyn

clean:
	rm -f *.o *.a *.so main