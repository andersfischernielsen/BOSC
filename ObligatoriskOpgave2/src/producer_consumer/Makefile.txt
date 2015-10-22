CC ?= cc
CFLAGS = -g
LDFLAGS = -pthread

producer_consumer: sleep.o producer_consumer.o ../list/list.o

clean:
	$(RM) producer_consumer *.o ../list/list.o
