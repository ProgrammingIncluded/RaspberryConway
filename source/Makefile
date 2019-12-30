BINARY=conway
CC=g++
BIN=../bin
DEPS=rle_loader.hpp quadtree.hpp app.hpp game.hpp opengl.hpp
OBJ=rle_loader.o quadtree.o app.o opengl.o

MESA_DIR=~/mesa
OPENGL_INCLUDE=${MESA_DIR}/include
OPENGL_LIB=${MESA_DIR}/lib

# Default to no opengl but write to framebuffer
OPENGLES?=0


ifeq (${OPENGLES}, 1)
OPENGL_FLAGS=-lGLESv2 -lEGL -lpthread -lgbm
endif

CFLAGS=-DOPENGLES_MODE=${OPENGLES} -std=c++11 -Wall ${OPENGL_FLAGS}

%.o: %.cpp $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS)

${BINARY}: ${OBJ} main.o
	${CC} ${CFLAGS} $^ -o ${BIN}/${BINARY} -pthread

${BINARY}_hashlife: ${OBJ} hashlife.o
	${CC} ${CFLAGS} $^ -o ${BIN}/${BINARY}_hashlife

run_hashlife: ${BINARY}_hashlife
	# Program should run in same directory binary with rle folder
	cd ${BIN}; sudo ./${BINARY}_hashlife

run: ${BINARY}
	# Program should run in same directory binary with rle folder
	cd ${BIN}; sudo ./${BINARY}

clean:
	rm *.o
	rm ${BIN}/${BINARY}
