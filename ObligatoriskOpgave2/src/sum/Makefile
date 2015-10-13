CC = cc -g
LIBS = -lm
OBJS = sum.o

sum: $(OBJS)
	$(CC) -pthread -o $@ $(OBJS) $(LIBS)

sum.o: sum.c
	$(CC) -c sum.c

clean:
	rm -f *.o sum
