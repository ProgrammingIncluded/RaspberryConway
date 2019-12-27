BINARY=conway
CC=g++
CFLAGS = -std=c++11 -Wall -I
BIN = ../bin
DEPS = rle_loader.hpp app.hpp
OBJ = rle_loader.o app.o main.o

%.0: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

${BINARY}: ${OBJ}
	${CC} ${CFLAGS} $@ $^ -o ${BIN}/${BINARY}

run: ${BINARY}
	# Program should run in same directory binary with rle folder
	cd ${BIN}; sudo ./${BINARY}

clean:
	rm *.o
	rm ${BIN}/${BINARY}