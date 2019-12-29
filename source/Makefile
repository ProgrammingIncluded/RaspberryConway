BINARY=conway
CC=g++
CFLAGS = -std=c++11 -Wall -I
BIN = ../bin
DEPS = rle_loader.hpp quadtree.hpp app.hpp game.hpp
OBJ = rle_loader.o quadtree.o app.o

%.0: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

${BINARY}_legacy: ${OBJ} main_legacy.o
	${CC} ${CFLAGS} $@ $^ -o ${BIN}/${BINARY}_legacy

${BINARY}: ${OBJ} main.o
	${CC} ${CFLAGS} $@ $^ -o ${BIN}/${BINARY}

run_legacy: ${BINARY}_legacy
	# Program should run in same directory binary with rle folder
	cd ${BIN}; sudo ./${BINARY}_legacy

run: ${BINARY}
	# Program should run in same directory binary with rle folder
	cd ${BIN}; sudo ./${BINARY}

clean:
	rm *.o
	rm ${BIN}/${BINARY}
