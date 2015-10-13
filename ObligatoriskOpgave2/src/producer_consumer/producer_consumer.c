#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "sleep.h"
#include "../list/list.h"

void *consumer(void *param);
void *producer(void *param);

List *buffer;
int item_number;
sem_t mutex, empty, full, itemnos;

int main(int argc, char* argv[]) {
	if (argc != 5) {
		printf("Usage:\n\tproducer_consumer <number of producers> <number of consumers> <size of shared buffers> <number of produced items>\n");
		return -1;
	}

	int producer_number = atoi(argv[1]);
	int consumer_number = atoi(argv[2]);
	int buffer_size = atoi(argv[3]);
	item_number = atoi(argv[4]);

	if (item_number == 0) {
		return 0;
	}

	buffer = list_new();
	
	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, buffer_size);
	sem_init(&full, 0, 0);
	sem_init(&itemnos, 0, 1);

	pthread_t *producers = malloc(producer_number * sizeof(pthread_t));
	pthread_t *consumers = malloc(consumer_number * sizeof(pthread_t));

	pthread_attr_t *attr = malloc(sizeof(pthread_attr_t));
	pthread_attr_init(attr);

	int i;
	for (i = 0; i < producer_number; i++) {
		int id = i;
		pthread_create(producers + i, attr, &producer, &id);
	}

	for (i = 0; i < consumer_number; i++) {
		int id = i;
		pthread_create(consumers + i, attr, &consumer, &id);
	}

	// Wait for threads.
	for (i = 0; i < producer_number; i++) {
		pthread_join(*(producers + i), NULL);
	}

	printf("Producers done\n");

	for (i = 0; i < consumer_number; i++) {
		pthread_join(*(consumers + i), NULL);
	}

	printf("Consumers done\n");

	free(producers);
	free(consumers);
	free(attr);
	free(buffer);
	sem_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);
	sem_destroy(&itemnos);
	
	return 0;
}

int item_no = 0;

void *producer(void *param) {
	int id = *((int*) param);
	int item;
	while (item_number-- > 0) {
		/* sleep for a random period of time */
		Sleep(200);
		
		char itemstr[100];

		sem_wait(&itemnos);
		sprintf(itemstr, "node number: %d", item_no++);
		sem_post(&itemnos);

		Node *n = node_new_str(itemstr);

		sem_wait(&empty);
		sem_wait(&mutex);

		list_add(buffer, n);

		sem_post(&mutex);
		sem_post(&full);

		printf("Producer %d produced %s. Items in buffer: %d\n", id, (char *) n->elm, buffer->len);
	}
	return NULL;
}

void *consumer(void *param) {
	int id = *((int*) param);
	int item;
	do {
		/* sleep for a random period of time */
		Sleep(200);

		sem_wait(&full);
		sem_wait(&mutex);

		Node *n = list_remove(buffer);
		if (n == NULL) {
			printf("Null Node found!!!\n");
		}
		
		sem_post(&mutex);
		sem_post(&empty);

		printf("\tConsumer %d consumed %s. Items in buffer: %d\n", id, (char *) n->elm, buffer->len);
	} while (item_number > 0 || buffer->len != 0);
	return NULL;
}
