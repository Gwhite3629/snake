CC=gcc
LINK=gcc
TARGET1=snake
TARGET2=test
OBJSgame=runtime.o snake.o
OBJStest=test.o snake.o
LIBS=-pthread
CFLAGS=-O3 -Wall -Wextra

all: ${TARGET1} ${TARGET2}

${TARGET1}: ${OBJSgame}
	${CC} -o ${TARGET1} ${OBJSgame} ${LIBS}

${TARGET2}: ${OBJStest}
	${CC} -o ${TARGET2} ${OBJStest} ${LIBS}

.PHONY : clean

clean:
	rm -f ${TARGET1} core*
	rm -f ${TARGET2} core*
	rm -f *.o core*