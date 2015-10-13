all: sumComp listComp producer-consumerComp bankerComp

CC = gcc
WINDOWS = *.exe

OBJS1 = ./sum/sum.c

OBJS2 = ./list/list.c ./list/main.c
LIBS2 = -lpthread

OBJS3 = ./producer-consumer/producer-consumer.c
LIBS3 = 

OBJS4 = ./banker/banker.c
LIBS4 = 

sumComp: ${OBJS1}
	${CC} -pthread -g ${OBJS1} -o ./sum/$@ -lm

listComp: ${OBJS2}
	${CC} -o ./list/$@ ${LIBS2} ${OBJS2}

producer-consumerComp: ${OBJS3}
	${CC} -o $@ ${OBJS3} ${LIBS3}

bankerComp: ${OBJS4}
	${CC} -o $@ ${OBJS4} ${LIBS4}

clean:
	rm -rf *o ${WINDOWS} ./sum/sumComp ./list/listComp ./producer-consumer/producer-consumerComp ./banker/bankerComp
