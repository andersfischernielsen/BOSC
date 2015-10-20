CC ?= cc
CFLAGS = -g
LDFLAGS = -pthread
LDLIBS = -lm

banker: banker.o ../producer_consumer/sleep.o

clean:
	$(RM) *.o banker ../producer_consumer/sleep.o
