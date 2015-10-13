#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "sleep.h"
#include "../list/list.h"

void *consumer(void *param);
void *producer(void *param);

List *buf;
int item_number;
sem_t empty, full;

void fill_arr(int *arr, int size) {
	int i;
	for (i = 0; i < size; i++) {
		arr[i] = i;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		printf("Usage:\n\tproducer_consumer <number of producers> <number of consumers> <size of shared buffers> <number of produced items>\n");
		return -1;
	}

	int producer_number = atoi(argv[1]);
	int consumer_number = atoi(argv[2]);
	int buffer_size = atoi(argv[3]);
	item_number = atoi(argv[4]);

	if (item_number == 0 || producer_number == 0 || consumer_number == 0 || item_number == 0) {
		return 0;
	}

	buf = list_new();
	
	sem_init(&empty, 0, buffer_size);
	sem_init(&full, 0, 0);

	pthread_t producers[producer_number];
	pthread_t consumers[consumer_number];

	int p_arr[producer_number];
	int c_arr[consumer_number];

	fill_arr(p_arr, producer_number);
	fill_arr(c_arr, consumer_number);

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	int i;
	for (i = 0; i < producer_number; i++) {
		pthread_create(producers + i, &attr, &producer, &p_arr[i]);
	}

	for (i = 0; i < consumer_number; i++) {
		pthread_create(consumers + i, &attr, &consumer, &c_arr[i]);
	}

	// Wait for threads.
	for (i = 0; i < producer_number; i++) {
		pthread_join(producers[i], NULL);
	}

	printf("Producers done\n");

	for (i = 0; i < consumer_number; i++) {
		pthread_join(consumers[i], NULL);
	}

	printf("Consumers done\n");

	// Free list buffer?
	sem_destroy(&empty);
	sem_destroy(&full);
	
	return 0;
}

int item_no = 0;

void *producer(void *param) {
	int id = *((int*) param);
	int item;
	char itemstr[100];
	while (item_number-- > 0) {
		sprintf(itemstr, "node number: %d", item_no++);

		sem_wait(&empty);
		Node *n = node_new_str(itemstr);
		list_add(buf, n);
		int len = buf->len;

		sem_post(&full);

		printf("Producer %d produced %s. Items in buffer: %d\n", id, itemstr, len);

		/* sleep for a random period of time */
		Sleep(200);
	}
	printf("Producer %d stopped\n", id);
	pthread_exit(NULL);
}

void *consumer(void *param) {
	int id = *((int*) param);
	int item;
	while (buf->len != 0 || item_number > 0) {
		if (buf->len != 0) {
			sem_wait(&full);
			int fullcount;
			sem_getvalue(&full, &fullcount);
			printf("Fullcount: %d\n", fullcount);

			Node *n = list_remove(buf);
			if (n == NULL) {
				printf("Null Node found!!!\n");
				return NULL;
			}
			int len = buf->len;
		
			sem_post(&empty);

			printf("\tConsumer %d consumed %s. Items in buffer: %d\n", id, (char *)n->elm, len);
			free(n->elm);
			free(n);
		}
		/* sleep for a random period of time */
		Sleep(200);
	}
	printf("Consumer %d stopped\n", id);
	pthread_exit(NULL);
}
