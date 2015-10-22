CC ?= cc

CFLAGS = -g
LDFLAGS = -pthread

all: fifo fifotest

fifo: main.o list.o
	$(LINK.c) -o $@ $^
	
fifotest: fifotest.o list.o
	$(LINK.c) -o $@ $^

clean:
	$(RM) *o fifo fifotest *.exe
