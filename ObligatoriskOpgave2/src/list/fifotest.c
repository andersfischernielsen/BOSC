
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

// FIFO list;
List *fifo;
int iterations;

void *runner(void *param);	/* threads call this function */
int main(int argc, char* argv[])
{
	if (!argv[1]) {
		printf("First argument should be number of iterations. \n");
		return -1;
	}
	if (!argv[2]) {
		printf("Second argument should be number of threads to start. \n");
		return -1;
	}
	fifo = list_new();
	iterations = atoi(argv[1]);
	int threads = atoi(argv[2]);				//Thread id array.
	pthread_t workers[threads];				//Thread identifiers.
	int *threadId = malloc(sizeof(int)*threads);

	int i;
	int err;
	for (i = 0; i < threads; i++) {
		//Create the threads.
		workers[i] = i;
		threadId[i] = i;
		err = pthread_create(&workers[i], NULL, &runner, &threadId[i]);
		if (err != 0)
            printf("\ncan't create thread :[%d]", strerror(err));
	}

	for (i = 0; i < threads; i++) {
		//Wait for the threads to exit.
		pthread_join(workers[i], NULL);
	}
	return 0;
}

void *runner(void *param)
{
	int thread = *((int *)(param));
	char thread_text1[40];
	char thread_text2[40];
	int i;
	for(i = 0; i < iterations; i++)
	{
		sprintf(thread_text1, "s1, thread: %d iteration: %d\n", thread, i);
		sprintf(thread_text2, "s2, thread: %d iteration: %d\n", thread, i);
		list_add(fifo, node_new_str(thread_text1));
		list_add(fifo, node_new_str(thread_text2));
		Node *n1 = list_remove(fifo);
		if (n1 == NULL) { printf("Error no elements in list\n"); pthread_exit(NULL);}
		Node *n2 = list_remove(fifo);
		if (n2 == NULL) { printf("Error no elements in list\n"); pthread_exit(NULL);}
		printf("%s\n%s\n", (char*) (n1->elm), (char*) (n2->elm));
	}
	return NULL;
}
