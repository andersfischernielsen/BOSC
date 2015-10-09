#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "sleep.h"

int main(int argc, char* argv[]) {
	if (argc != 5) {
		printf("Usage:\n\tproducer_consumer <number of producers> <number of consumers> <size of shared buffers> <number of produced items>\n");
		return -1;
	}
	int producer_number = atoi(argv[1]);
	int consumer_number = atoi(argv[2]);
	int buffer_size = atoi(argv[3]);
	int item_number = atoi(argv[4]);
	Sleep(1000);

	sem_t empty;

	sem_init(&empty, 0, buffer_size);
	sem_wait(&empty);
	printf("Producers: %d\nConsumers: %d\nBuffer Size: %d\nNumber of items: %d\n", producer_number, consumer_number, buffer_size, item_number);
	sem_post(&empty);
	int sem_count;
	sem_getvalue(&empty, &sem_count);
	printf("Semaphore number: %d\n", sem_count);
	sem_destroy(&empty);

	return 0;
}
