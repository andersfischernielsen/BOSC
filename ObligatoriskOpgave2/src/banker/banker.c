#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../producer_consumer/sleep.h"

typedef struct state {
	int *resource;
	int *available;
	int **max;
	int **allocation;
	int **need;
} State;

// Global variables
int m, n;
State *s = NULL;

//Function signatures
void free_state(State *s);

State *allocate_state();

// Mutex for access to state.
pthread_mutex_t state_mutex;

void print_array(int *toPrint, int length) {
	int i;
	printf("{ ");
	for (i = 0; i < length; i++) {
		printf("%i ", toPrint[i]);
	}
	printf("} \n");
}

int less_eq_int_arr(int *a, int *b, int length) {
	int i;
	for (i = 0; i < length; i++) {
		if (a[i] > b[i]) {
			return 0;
		}
	}
	return 1;
}

int is_state_safe(State *state) {
	// Step 1
	int finish[m];

	int i;

	for (i = 0; i < m; i++) {
		finish[i] = 0;
	}

	int work[n];

	memcpy(work, state->available, n * sizeof(int));

	// Step 2
	int j;
	for (i = 0; i < m; i++) {
		if (!finish[i] && less_eq_int_arr(state->need[i], work, n)) {
			// Step 3
			for (j = 0; j < n; j++) {
				work[j] += state->allocation[i][j];
			}
			finish[i] = 1;
			i = -1; // Check all again.
		}
	}

	// Step 4
	for (i = 0; i < n; i++) {
		if (!finish[i]) {
			return 0;
		}
	}
	return 1;
}

/* Allocate resources in request for process i, only if it 
   results in a safe state and return 1, else return 0 */
int resource_request(int i, int *request) {
	pthread_mutex_lock(&state_mutex);

	// Step 1 in book
	// Iterate through the request.
	if (!less_eq_int_arr(request, s->need[i], n)) {
		printf("Process %d has requested too much!\nRequest: ", i);
		print_array(request, n);
		printf("Need: ");
		print_array(s->need[i], n);
		// Should probably stop the process instead.
		//Return unsafe state.
		pthread_mutex_unlock(&state_mutex);
		return 0;
	}

	// Step 2 in book
	if (!less_eq_int_arr(request, s->available, n)) {
		//Return unsafe state.
		pthread_mutex_unlock(&state_mutex);
		return 0;
	}

	// Step 3 in book
	int j;
	for (j = 0; j < n; j++) {
		s->available[j] -= request[j];
		s->allocation[i][j] += request[j];
		s->need[i][j] -= request[j];
	}

	if (!is_state_safe(s)) {
		for (j = 0; j < n; j++) {
			s->available[j] += request[j];
			s->allocation[i][j] -= request[j];
			s->need[i][j] += request[j];
		}

		//Return unsafe state.
		pthread_mutex_unlock(&state_mutex);
		return 0;
	}
	//Else return safe state. Make s point to the correct "test state".
	printf("Available after allocation of request %d: ", i);
	print_array(s->available, n);
	pthread_mutex_unlock(&state_mutex);
	return 1;
}

/* Release the resources in request for process i */
void resource_release(int i, int *request) {
	pthread_mutex_lock(&state_mutex);
	int j;
	for (j = 0; j < n; j++) {
		//Update allocated resources.
		s->allocation[i][j] -= request[j];
		//Update available resources counter.
		s->available[j] += request[j];
	}
	pthread_mutex_unlock(&state_mutex);
}

/* Generate a request vector */
void generate_request(int i, int *request) {
	int j, sum = 0;
	while (!sum) {
		for (j = 0; j < n; j++) {
			request[j] = (int) ceil(s->need[i][j] * ((double) rand()) / (double) RAND_MAX);
			sum += request[j];
		}
	}
	printf("Process %d: Requesting resources.\n", i);
}

/* Generate a release vector */
void generate_release(int i, int *request) {
	int j, sum = 0;
	while (!sum) {
		for (j = 0; j < n; j++) {
			request[j] = (int) ceil(s->allocation[i][j] * ((double) rand()) / (double) RAND_MAX);
			sum += request[j];
		}
	}
	printf("Process %d: Releasing resources.\n", i);
}

int is_need_empty(int i) {
	int j;
	for (j = 0; j < n; j++) {
		if (s->need[i][j])
			return 0;
	}
	return 1;
}

/* Threads starts here */
void *process_thread(void *param) {
	/* Process number */
	int i = (int) (long) param;
	/* Allocate request vector */
	int *request = malloc(n * sizeof(int));

	while (!is_need_empty(i)) {
		/* Generate request */
		generate_request(i, request);
		while (!resource_request(i, request)) {
			printf("Request %d waiting\n", i);
			/* Wait */
			Sleep(100);
		}
		/* Generate release */
		generate_release(i, request);
		/* Release resources */
		resource_release(i, request);
		printf("Process %d: Resources released.\n", i);
		/* Wait */
		Sleep(1000);
	}
	free(request);
	printf("Process %d finished\n", i);
	return NULL;
}

int **allocate_int_matrix(int m, int n) {
	/* Allocate memory for the elements */
	int *mem = malloc(m * n * sizeof(int));

	if (!mem) {
		printf("Out of memory!\n");
		exit(-1);
	}

	/* Allocate memory for the matrix array */
	int **mat = malloc(m * sizeof(int *));

	if (!mat) {
		free(mem);
		printf("Out of memory!\n");
		exit(-1);
	}

	/* Setup array */
	int i;
	for (i = 0; i < m; ++i) {
		mat[i] = &mem[i * n];
	}
	return mat;
}

State *allocate_state() {
	State *toReturn;
	toReturn = malloc(sizeof(State));

	if (!toReturn) {
		printf("Out of memory!\n");
		exit(-1);
	}

	toReturn->resource = calloc((size_t) n, sizeof(int));
	toReturn->available = calloc((size_t) n, sizeof(int));
	toReturn->max = allocate_int_matrix(m, n);
	toReturn->allocation = allocate_int_matrix(m, n);
	toReturn->need = allocate_int_matrix(m, n);

	return toReturn;
}

void free_state(State *s) {
	free(s->resource);
	free(s->available);
	free(*s->max);
	free(*s->allocation);
	free(*s->need);
	free(s->max);
	free(s->allocation);
	free(s->need);
	free(s);
}

int main(int argc, char *argv[]) {
	/* Get size of current state as input */
	int i, j;
	printf("Number of processes: ");
	scanf("%d", &m);
	printf("Number of resources: ");
	scanf("%d", &n);

	/* Allocate memory for state */
	s = allocate_state();

	if (s == NULL) {
		printf("\nYou need to allocate memory for the state!\n");
		exit(0);
	};

	/* Get current state as input */
	printf("Resource vector: ");
	for (i = 0; i < n; i++)
		scanf("%d", s->resource + i);
	printf("Enter max matrix: ");
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			scanf("%d", s->max[i] + j);
	printf("Enter allocation matrix: ");
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			scanf("%d", s->allocation[i] + j);
		}
	}
	printf("\n");

	/* Calculate the need matrix */
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			s->need[i][j] = s->max[i][j] - s->allocation[i][j];

	/* Calculate the availability vector */
	for (j = 0; j < n; j++) {
		int sum = 0;
		for (i = 0; i < m; i++)
			sum += s->allocation[i][j];
		s->available[j] = s->resource[j] - sum;
	}

	/* Output need matrix and availability vector */
	printf("Need matrix:\n");
	for (i = 0; i < n; i++)
		printf("R%d ", i + 1);
	printf("\n");
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++)
			printf("%d  ", s->need[i][j]);
		printf("\n");
	}
	printf("Availability vector:\n");
	for (i = 0; i < n; i++)
		printf("R%d ", i + 1);
	printf("\n");
	for (j = 0; j < n; j++)
		printf("%d  ", s->available[j]);
	printf("\n");

	/* If initial state is unsafe then terminate with error */
	if (!is_state_safe(s)) {
		printf("Initial state is not safe!\n");
		exit(40);
	}

	/* Create m threads */
	pthread_t *tid = malloc(m * sizeof(pthread_t));
	for (i = 0; i < m; i++)
		pthread_create(tid + i, NULL, process_thread, (void *) (long) i);

	/* Wait for threads to finish */
	for (i = 0; i < m; i++)
		pthread_join(tid[i], NULL);

	free(tid);

	/* Free state memory */
	free_state(s);

	return 0;
}

