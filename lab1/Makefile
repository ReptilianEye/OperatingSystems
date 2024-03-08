CXX = gcc
CFLAGS = -Wall -g

.PHONY = all clean

all: countdown

countdown: countdown.c
	${CXX} ${CFLAGS} countdown.c -o countdown

clean:
	find . -type f -executable -delete
	@echo "Binaries deleted"
