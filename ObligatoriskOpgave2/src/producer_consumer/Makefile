CC = cc
OBJS = sleep.o producer_consumer.o

producer_consumer: $(OBJS)
	$(CC) -pthread -o $@ $(OBJS)

sleep.o: sleep.c
	$(CC) -c sleep.c

producer_consumer.o: producer_consumer.c sleep.h
	$(CC) -c producer_consumer.c

clean:
	rm -f producer_consumer *.o
