#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

int first = 0;

/* Random sleep function */
void Sleep(float wait_time_ms)
{
	if(first == 0) {
	// First time setup - make the seed depend on
	// the current time, in order not to use the
	// same sequence of "random" numbers every
	// time the program is used.
		first = 1;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		srand(tv.tv_usec);
	}
	wait_time_ms = ((float)rand())*wait_time_ms / (float)RAND_MAX;
	usleep((int) (wait_time_ms * 1e3f)); // convert from ms to us
}
