all: list1 banker1 sum1 producer_consumer1

CC = gcc
WINDOWS = *.exe

list1:
	$(MAKE) -C list

banker1:
	$(MAKE) -C banker

sum1:
	$(MAKE) -C sum

producer_consumer1:
	$(MAKE) -C producer_consumer

clean:
	rm -rf *o ${WINDOWS}
	for d in sum list banker producer_consumer; \
	do \
	$(MAKE) --directory=$$d clean; \
	done
