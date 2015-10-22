#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

typedef struct thread_data {
	int from;
	int to;
	double sum;
} thread_data;

// Function declarations. //
void *runner(void *param);
double sum_array(thread_data a[], int num_elements);
thread_data *generate_work_structs();
void spawn_threads_and_execute(thread_data *work);


// Globals //
int iterations;
int threads;
int i;


// Implementation. //
int main(int argc, char* argv[]) {
	if (!argv[1]) {
		printf("First argument should be number of iterations. \n");
		exit(-1);
	}
	if (!argv[2]) {
		printf("Second argument should be number of threads to start. \n");
		exit(-2);
	}

	iterations = atoi(argv[1]);
	threads = atoi(argv[2]);

	thread_data *work = generate_work_structs();
	spawn_threads_and_execute(work);
	double sum = sum_array(work, threads);

	printf("sum = %G \n", sum);
	free(work);
	return 0;
}

thread_data *generate_work_structs() {
	//Partition for each thread (ex. 10000/4 = 2500 for each thread).
	int partition_size = iterations/threads;

	//Struct containing work amount.
	thread_data *work = malloc(sizeof(thread_data)*threads);	

	//Init structs for work.
	for (i = 0; i < threads; i++) {
		//Init interval in structs. 
		//Example: 
		//partition_size = 2500
		//First struct:  { from = 2500*0 = 0,    to = 2500*1 = 2500   }
		//Second struct: { from = 2500*1 = 2500, to = 2500 * 2 = 5000 } etc.
		thread_data data = { partition_size * i, partition_size * (i + 1), 0 };
		//Store in array of work.
		work[i+1] = data;
	}

	return work;
}

void spawn_threads_and_execute(thread_data *work) {
	//Thread identifiers.
	pthread_t workers[threads];

	for (i = 0; i < threads; i++) {
		//Make identifiers different from each other.
		workers[i] = i;
		//Instantiate threads and start work with a struct for each thread.
		pthread_create(&workers[i], NULL, runner, work + i);
	}

	for (i = 0; i < threads; i++) {
		//Wait for the threads to exit.
		pthread_join(workers[i], NULL);
	}
}

//Given a work struct, sum the square roots of every integer in the 
//work structs interval (eg. from: 500 to: 1000 will sqrt 501 to 1000).
void *runner(void *param) {
	thread_data *data = (thread_data *) param;
	// Store result locally to avoid copying between CPU caches (slow).
	double localsum = 0; 
	int j;
	for (j = data->from + 1; j <= data->to; j++) {
		//Calculate square root and add to sum.
		localsum += sqrt(j);
	}
	//Store calculated sum in work struct.
	data->sum = localsum;
	pthread_exit(0);
}

//Iterate over array and sum struct sums. 
//Return sum of struct sums.
double sum_array(thread_data a[], int num_elements) {
	double sum = 0;
	for (i = 0; i < num_elements; i++) {
		sum = sum + a[i].sum;
	}

	return sum;
}
