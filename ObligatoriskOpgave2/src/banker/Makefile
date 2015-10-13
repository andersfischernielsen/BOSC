CC = cc -g
OBJS = banker.o

banker: $(OBJS)
	$(CC) -pthread -o $@ $(OBJS)

banker.o: banker.c
	$(CC) -c banker.c

clean:
	rm -f *.o banker
