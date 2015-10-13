all: fifo fifoTest

OBJS = list.c main.c
OBJSTEST = list.c fifotest.c
LIBS= -lpthread

fifo: ${OBJS}
	gcc -o $@  ${LIBS} ${OBJS}
	
fifoTest: ${OBJSTEST}
	gcc -o $@  ${LIBS} ${OBJSTEST}

clean:
	rm -rf *o fifo *.exe
