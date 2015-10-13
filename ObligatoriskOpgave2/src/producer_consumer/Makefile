CC = cc -g
OBJS = sleep.o producer_consumer.o list.o

producer_consumer: $(OBJS)
	$(CC) -pthread -o $@ $(OBJS)

sleep.o: sleep.c
	$(CC) -c sleep.c

producer_consumer.o: producer_consumer.c sleep.h ../list/list.h
	$(CC) -c producer_consumer.c

list.o: ../list/list.c
	$(CC) -c ../list/list.c

clean:
	rm -f producer_consumer *.o
