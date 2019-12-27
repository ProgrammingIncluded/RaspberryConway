BINARY=conway
CC=g++
CFLAGS = -std=c++11 -Wall -I
BIN = ../bin
DEPS = app.hpp
OBJ = app.o main.o

%.0: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

${BINARY}: ${OBJ}
	${CC} ${CFLAGS} $@ $^ -o ${BIN}/${BINARY}

run: ${BINARY}
	sudo ${BIN}/${BINARY}

clean:
	rm *.o
	rm ${BIN}/${BINARY}