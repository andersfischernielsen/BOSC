#include <stdio.h>
#include <pthread.h>

int sum; /* this data is shared by the thread(s) */
void *runner(void *param); /* threads call this function */

int main(int argc, char* argv[]) {
	pthread t tid; //Thread identifier.
	pthread attr t attr; //Thread attributes.

	//Get the default attributes.
	pthread attr init(&attr);

	//Create the thread.
	pthread create(&tid, &attr, runner, argv[1]); //Wait for the thread to exit.
	pthread join(tid, NULL); 

	printf("sum = %d\n", sum);

	return 0;
}

//Function to give to thread to execute.
void *runner(void *param)
{
 	int i, upper = atoi(param);
 	
 	sum = 0;
 	for (i = 1; i <= upper; i++) {
       sum += i;
 	}

	pthread_exit(0);
}
