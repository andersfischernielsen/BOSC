#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "sleep.h"
#include "../list/list.h"

void *consumer(void *param);
void *producer(void *param);

List *buf;
int buffer_size;
sem_t empty, full;

typedef struct {
	int id;
	int from;
	int to;
} prod_t;

typedef struct {
	int id;
	int amount;
} cons_t;

int main(int argc, char* argv[]) {
	if (argc != 5) {
		printf("Usage:\n\tproducer_consumer <number of producers> <number of consumers> <size of shared buffers> <number of produced items>\n");
		return -1;
	}

	int producer_number = atoi(argv[1]);
	int consumer_number = atoi(argv[2]);
	buffer_size = atoi(argv[3]);
	int item_number = atoi(argv[4]);

	if (item_number == 0 || producer_number == 0 || consumer_number == 0 || item_number == 0) {
		return 0;
	}

	buf = list_new();

	sem_init(&empty, 0, buffer_size);
	sem_init(&full, 0, 0);

	pthread_t producers[producer_number];
	pthread_t consumers[consumer_number];

	prod_t prod_work[producer_number];
	cons_t cons_work[consumer_number];

	int i;
	for (i = 0; i < producer_number; i++) {
		/* Create the work for a producer */
		prod_work[i].id = i;
		prod_work[i].from = i * (item_number / producer_number);
		if (i == producer_number - 1) {
			prod_work[i].to = item_number;
		} else {
			prod_work[i].to = (i+1) * (item_number / producer_number);
		}

		/* Run the producer */
		pthread_create(producers + i, NULL, &producer, prod_work + i);
	}

	for (i = 0; i < consumer_number; i++) {
		/* Create the work for a consumer */
		cons_work[i].id = i;
		if (i == consumer_number - 1) {
			cons_work[i].amount = item_number / consumer_number + item_number % consumer_number;
		} else {
			cons_work[i].amount = item_number / consumer_number;
		}

		/* Run the consumer */
		pthread_create(consumers + i, NULL, &consumer, cons_work + i);
	}

	// Wait for threads.
	for (i = 0; i < producer_number; i++) {
		pthread_join(producers[i], NULL);
	}

	for (i = 0; i < consumer_number; i++) {
		pthread_join(consumers[i], NULL);
	}

	// Might mean that some memory inside the buffer is never freed.
	free(buf);

	sem_destroy(&empty);
	sem_destroy(&full);

	return 0;
}

void *producer(void *param) {
	prod_t work = *((prod_t*) param);
	char itemstr[100];
	int i;
	for (i = work.from; i < work.to; i++) {
		sem_wait(&empty);
		sprintf(itemstr, "item %d", i);
		Node *n = node_new_str(itemstr);
		list_add(buf, n);
		int len = buf->len;

		printf("Producer %d produced %s. Items in buffer: %d (Max: %d)\n", work.id, itemstr, len, buffer_size);

		sem_post(&full);
		/* sleep for a random period of time */
		Sleep(200);
	}
	pthread_exit(NULL);
}

void *consumer(void *param) {
	cons_t work = *((cons_t*) param);
	int i;
	for (i = 0; i < work.amount; i++) {
		sem_wait(&full);

		Node *n = list_remove(buf);
		int len = buf->len;

		printf("\tConsumer %d consumed %s. Items in buffer: %d (Max: %d)\n", work.id, (char *)n->elm, len, buffer_size);
		free(n->elm);
		free(n);

		sem_post(&empty);

		/* sleep for a random period of time */
		Sleep(200);
	}
	pthread_exit(NULL);
}
