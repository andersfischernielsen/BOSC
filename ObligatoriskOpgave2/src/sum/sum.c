#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

float sum; /* this data is shared by the thread(s) */
void *runner(void *param); /* threads call this function */

int main(int argc, char* argv[]) {
	pthread_t tid; //Thread identifier.
	pthread_attr_t attr; //Thread attributes.

	//Get the default attributes.
	pthread_attr_init(&attr);

	//Create the thread.
	pthread_create(&tid, &attr, runner, argv[1]); //Wait for the thread to exit.
	pthread_join(tid, NULL); 

	printf("sum = %f \n", sum);

	return 0;
}

//Function to give to thread to execute.
void *runner(void *param)
{
 	int i;
 	int upper = atoi(param);
 	
 	sum = 0;
 	for (i = 1; i <= upper; i++) {
       sum += sqrt(i);
 	}

	pthread_exit(0);
}
