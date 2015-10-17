#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

typedef struct thread_data {
	int from;
	int to;
	double sum;
} thread_data;

void *runner(void *param);	/* threads call this function */
double sum_array(thread_data a[], int num_elements);

int main(int argc, char* argv[]) {
	if (!argv[1]) {
		printf("First argument should be number of iterations. \n");
		exit(-1);
	}
	if (!argv[2]) {
		printf("Second argument should be number of threads to start. \n");
		exit(-2);
	}


	int threads = atoi(argv[2]);				//Thread id array.
	int partition_size = atoi(argv[1])/threads; //Partition for each thread (ex. 10000/4 = 2500 for each thread).

	pthread_t workers[threads];				//Thread identifiers.
	thread_data* work = malloc(sizeof(thread_data)*threads);	//Struct containing work amount.

	//Init structs for work.
	int i;
	for (i = 0; i < threads; i++) {
		thread_data data = { partition_size * i, partition_size * (i +1), 0 };
		*(work+i) = data;
	}

	for (i = 0; i < threads; i++) {
		//Create the threads.
		workers[i] = i;
		pthread_create(&workers[i], NULL, runner, work + i);
	}

	for (i = 0; i < threads; i++) {
		//Wait for the threads to exit.
		pthread_join(workers[i], NULL);
	}
	double sum = sum_array(work, threads);

	printf("sum = %G \n", sum);
	free(work);
	return 0;
}

//Function to give to thread to execute.
void *runner(void *param)
{
	thread_data *data = (thread_data *) param;
	double localsum = 0; // TODO: Skriv noget om cpu caching.
	int i;
	for (i = data->from + 1; i <= data->to; i++) {
		//Iterate and store sum in struct.
		localsum += sqrt(i);
	}
	data->sum = localsum;
	pthread_exit(0);
}

//Function to iterate over array and sum struct sums.
double sum_array(thread_data a[], int num_elements)
{
	int i;
	double sum = 0;
	for (i = 0; i < num_elements; i++)
	{
		sum = sum + a[i].sum;
	}

	return sum;
}
