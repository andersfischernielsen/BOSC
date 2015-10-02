all: opgave1 opgave2 opgave3 opgave4

CC = gcc
WINDOWS = *.exe

OBJS1 = opgave1.o
LIBS1 = 

OBJS2 = opgave2.o
LIBS2 = 

OBJS3 = opgave3.o
LIBS3 = 

OBJS4 = opgave4.o
LIBS4 = 


opgave1: ${OBJS1}
	${CC} -o $@ ${OBJS1} ${LIBS1}

opgave2: ${OBJS2}
	${CC} -o $@ ${OBJS2} ${LIBS2}

opgave3: ${OBJS3}
	${CC} -o $@ ${OBJS3} ${LIBS3}

opgave4: ${OBJS4}
	${CC} -o $@ ${OBJS4} ${LIBS4}

clean:
	rm -rf *o ${WINDOWS} opgave1 opgave2 opgave3 opgave4
