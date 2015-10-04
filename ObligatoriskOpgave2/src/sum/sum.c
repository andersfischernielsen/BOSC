#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

double sums[8]; 			//Results of summarisation. TODO: Support more than 8 threads.
void *runner(void *param); 	/* threads call this function */
double sum_array(double a[], int num_elements);

int main(int argc, char* argv[]) {
	int threads = atoi(argv[2]); 				//Thread id array.
	int partition = atoi(argv[1])/threads; 		//Partition to work on for each thread. 
												//Ex.: 10000/4 threads = 2500 for each thread. 

	pthread_t workers[threads]; 	//Thread identifiers.


	//*********************************************************************
	//TODO: Following code doesn't work. It needs to be restructured.
	//We need to implement a struct containing either results or where to store the results, 
	//then pass the struct to the runner function. 

	//It's a start, though.


	int i;					//
	int toSum[2] = { partition, 0 };	//How many times to run and on which index in sums[] to store result.
	for (i = 0; i < threads; i++) {
		//Create the threads.
		pthread_create(&workers[i], NULL, runner, toSum); 
		toSum[1] = i*partition; 		//Set the index for next iteration. 
										//Ex.: 2500 for each thread, 
										//first time i = 0, 
										//second i = 2500, third i = 5000 etc.
	}

	//*********************************************************************
	

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
 	int i;
 	int upper = param[0];
 	int result_index = param[1];
 	
 	for (i = 1; i <= upper; i++) {
       	sums[result_index + i] += sqrt(i);
 	}

	pthread_exit(0);
}

double sum_array(double a[], int num_elements)
{
   	int i;
	double sum = 0;
   	for (i = 0; i < num_elements; i++)
   	{
	 	sum = sum + a[i];
   	}
   
   	return(sum);
}
