CC = cc -g
LIBS = -lm

sum: sum.o
	$(CC) -pthread -o sum sum.o $(LIBS)

sum.o: sum.c
	$(CC) -c sum.c

clean:
	rm -f *.o sum
