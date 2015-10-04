all: sum list producer-consumer banker

CC = gcc
WINDOWS = *.exe

OBJS1 = ./sum/sum.o
LIBS1 = 

OBJS2 = ./list/list.o
LIBS2 = 

OBJS3 = ./producer-consumer/producer-consumer.o
LIBS3 = 

OBJS4 = ./banker/banker.o
LIBS4 = 

sum: ${OBJS1}
	${CC} -o $@ ${OBJS1} ${LIBS1} -lm

list: ${OBJS2}
	${CC} -o $@ ${OBJS2} ${LIBS2}

producer-consumer: ${OBJS3}
	${CC} -o $@ ${OBJS3} ${LIBS3}

banker: ${OBJS4}
	${CC} -o $@ ${OBJS4} ${LIBS4}

clean:
	rm -r *o ${WINDOWS} sum list producer-consumer banker
