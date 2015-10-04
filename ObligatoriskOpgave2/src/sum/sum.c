#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

double sums[8]; 			//Results of summarisation. TODO: Support more than 8 threads.
void *runner(void *param); 	/* threads call this function */
double sum_array(double a[], int num_elements);

typedef struct thread_data { 
	int from;
	int to;
	double sum;
} t_data

int main(int argc, char* argv[]) {
	int threads = atoi(argv[2]); 				//Thread id array.
	int partition = atoi(argv[1])/threads; 		//Partition to work on for each thread. 
												//Ex.: 10000/4 threads = 2500 for each thread. 

	pthread_t workers[threads]; 				//Thread identifiers.
	thread_data[threads] work;

	//Create structs for work.
	int i;
	for (i = 0; i < threads; i++) {
		thread_data[i] = t_data;
		thread_data[i].from = partition * i;
		thread_data[i].to = partition * (i + 1);
	}

	int i;
	for (i = 0; i < threads; i++) {
		//Create the threads.
		pthread_create(&workers[i], NULL, runner, work[i]); 
	}

	for (i = 0; i < threads; i++) {
		pthread_join(workers[i], NULL); //Wait for the threads to exit.
	}

	double sum = sum_array(sums, 8);

	printf("sum = %G \n", sum);
	return 0;
}

//Function to give to thread to execute.
void *runner(void *param)
{
	thread_data task = *((thread_data *) param);
 	int upper = task->to;
 	
 	int i;
 	for (i = task->from + 1; i <= upper; i++) {
       	task.sum += sqrt(i);
 	}

	pthread_exit(0);
}

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
