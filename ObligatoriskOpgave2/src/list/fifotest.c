
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

// FIFO list;
List *fifo;
int iterations;
char **results;

void *runner(void *param);	/* threads call this function */
void *check_result(int array_length);	/* threads call this function */
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
	int result_size = threads*iterations*2;
	
	results = malloc(sizeof(char)*result_size);
	//prepare result array
	int i;
	for(i = 0; i<result_size; i++)
	{
		results[i] = malloc(sizeof(char*)*18);
	}
	int err;
	for (i = 0; i < threads; i++) {
		//Create the threads.
		workers[i] = i;
		threadId[i] = i;
		err = pthread_create(&workers[i], NULL, &runner, &threadId[i]);
		if (err != 0)
		{
            printf("\ncan't create thread :[%d]", strerror(err));
			exit(err);
		}
	}

	for (i = 0; i < threads; i++) {
		//Wait for the threads to exit.
		pthread_join(workers[i], NULL);
	}
	check_result(result_size);
	return 0;
}

void *runner(void *param)
{
	int thread = *((int *)(param));
	int start_index = thread*iterations*2;
	char thread_text[12];
	sprintf(thread_text, "thread: %d", thread);
	char thread_text1[40];
	char thread_text2[40];
	int i;
	int offset = 0;
	for(i = 0; i < iterations; i++)
	{
		sprintf(thread_text1, "s1, t: %d i: %d\n", thread, i);
		sprintf(thread_text2, "s2, t: %d i: %d\n", thread, i);
		list_add(fifo, node_new_str(thread_text1));
		list_add(fifo, node_new_str(thread_text2));
		
		// element 1 remove
		Node *n1 = list_remove(fifo);
		if (n1 == NULL) { printf("Error no elements in list\n"); pthread_exit(NULL);}
		char *result1 = malloc(sizeof(char)*18);
		sprintf(result1, "%s", (char*) (n1->elm));
		printf("%s removes: %s\n", thread_text, result1);
		results[start_index+offset] = result1;
		offset++;
		
		// element 2 remove
		Node *n2 = list_remove(fifo);
		if (n2 == NULL) { printf("Error no elements in list\n"); pthread_exit(NULL);}
		char *result2 = malloc(sizeof(char)*18);
		sprintf(result2, "%s", (char*) (n2->elm));
		printf("%s removes: %s\n", thread_text, result2);
		results[start_index+offset] = result2;
		offset++;
	}
	return NULL;
}

void *check_result(int array_length)
{
	int i;
	for(i = 0; i < array_length ; i++)
	{
		int j;
		char *result1 = results[i];
		for(j = i+1; j < array_length ; j++)
		{
			char *result2 = results[j];
			if(!strcmp(result1, result2))
			{
				printf("\nERROR - a race condition happened\n");
				printf("On index: %d and %d\n", i, j);
				printf("%s%s\n", result1, result2);
				exit(-1);
			}
		}
	}
	printf("\nEverything went fine\n");
}