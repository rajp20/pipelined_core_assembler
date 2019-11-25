CC = g++
CFLAGS = -Wall -std=c++11

assembler:
	${CC} ${CFLAGS} -o assembler assembler.cpp

clean:
	rm assembler
