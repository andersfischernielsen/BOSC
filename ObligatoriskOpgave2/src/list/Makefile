CC = cc -g

all: fifo fifotest

OBJS = list.o main.o
OBJSTEST = list.o fifotest.o

fifo: ${OBJS}
	$(CC) -pthread -o $@ ${OBJS}
	
fifotest: ${OBJSTEST}
	$(CC) -pthread -o $@ ${OBJSTEST}

fifotest.o: fifotest.c
	$(CC) -c fifotest.c

main.o: main.c
	$(CC) -c main.c

list.o: list.c
	$(CC) -c list.c

clean:
	rm -rf *o fifo fifotest *.exe
