#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

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

// Mutex for access to state.
pthread_mutex_t state_mutex;

/* Random sleep function */
void Sleep(float wait_time_ms)
{
    // add randomness
    wait_time_ms = ((float)rand())*wait_time_ms / (float)RAND_MAX;
    usleep((int) (wait_time_ms * 1e3f)); // convert from ms to us
}

/* Allocate resources in request for process i, only if it 
   results in a safe state and return 1, else return 0 */
int resource_request(int i, int *request)
{
    //1. Can the request be granted? //
    int j;
    //Iterate through the request.
    for (j = 0; j < m; j++) {                     
        //If the request exceeds the available resources, the request cannot be fulfilled.
        if (s->available[j] - request[j] < 0) {
            //Return unsafe state.
            return 0;                             
        }
    }

    //2. Assume that the request is granted. //
    for (j = 0; j < m; j++) {
        //Allocate resources.
        s->allocation[i][j] += request[j];	 
        //Update available resources counter.
        s->available[j] -= request[j];                 

        //Check that the allocation does not exceed max resources.
        //If so, return unsafe state.
        if (s->allocation[i][j] > s->max[i][j]) {       
            return 0;                                  
        }
    }

    int k;
    //3. Is the new state safe? //
    for (j = 0; j < m; j++) {
       for(k = 0; k < n; k++) {
            //Calculate need matrix. 
            s->need[j][k] = s->max[j][k] - s->allocation[i][j];  
            //Check that the needed resources for each process
            //doesn't exceed the available resources. If it does, unsafe state.
            if (s->need[j][k] > s->available[j]) {               
                return 0;                                        
            }
            //Else return safe state.
            return 1;
        }
    }
}

/* Release the resources in request for process i */
void resource_release(int i, int *request)
{
    int j;
    for (j = 0; j < m; j++) {
        //Update allocated resources.
       	s->allocation[i][j] -= request[j];
        //Update available resources counter.
        s->available[j] += request[j];
    }
}

/* Generate a request vector */
void generate_request(int i, int *request)
{
    int j, sum = 0;
    while (!sum) {
        for (j = 0;j < n; j++) {
            request[j] = s->need[i][j] * ((double)rand())/ (double)RAND_MAX;
            sum += request[j];
        }
    }
    printf("Process %d: Requesting resources.\n",i);
}

/* Generate a release vector */
void generate_release(int i, int *request)
{
    int j, sum = 0;
    while (!sum) {
        for (j = 0;j < n; j++) {
            request[j] = s->allocation[i][j] * ((double)rand())/ (double)RAND_MAX;
            sum += request[j];
        }
    }
    printf("Process %d: Releasing resources.\n",i);
}

/* Threads starts here */
void *process_thread(void *param)
{
    /* Process number */
    int i = (int) (long) param, j;
    /* Allocate request vector */
    int *request = malloc(n*sizeof(int));
    while (1) {
        /* Generate request */
        generate_request(i, request);
        while (!resource_request(i, request)) {
            /* Wait */
            Sleep(100);
        }
        /* Generate release */
        generate_release(i, request);
        /* Release resources */
        resource_release(i, request);
        /* Wait */
        Sleep(1000);
    }
    free(request);
}

State* allocate_state() {
    State* toReturn;
    toReturn = malloc(sizeof(State));

    toReturn->resource = (int *)calloc(n, sizeof(int));
    toReturn->available = (int*)calloc(n, sizeof(int));
    toReturn->max = (int **)malloc(m * sizeof(int*));
    toReturn->allocation = (int**)malloc(m * sizeof(int*));
    toReturn->need = (int**)malloc(m * sizeof(int*));
    
    int i;
    for(i = 0; i < m; i++) {
        toReturn->max[i] = (int*) calloc(n, sizeof(int));
        toReturn->allocation[i] = (int*)calloc(n, sizeof(int));
        toReturn->need[i] = (int*)calloc(n, sizeof(int));
    }
    
    return toReturn;
}

void free_state(State* s) {
    free(s->resource);
    free(s->available);
    int i;
    for(i = 0; i < m; i++) {
        free(s->max[i]);
        free(s->allocation[i]);
        free(s->need[i]);
    }
    free(s->max);
    free(s->allocation);
    free(s->need);
    free(s);
}

int main(int argc, char* argv[])
{
    /* Get size of current state as input */
    int i, j;
    printf("Number of processes: ");
    scanf("%d", &m);
    printf("Number of resources: ");
    scanf("%d", &n);
    
    /* Allocate memory for state */
    s = allocate_state();
    
    if (s == NULL) { printf("\nYou need to allocate memory for the state!\n"); exit(0); };
    
    /* Get current state as input */
    printf("Resource vector: ");
    for(i = 0; i < n; i++)
        scanf("%d", s->resource + i);
    printf("Enter max matrix: ");
    for(i = 0;i < m; i++)
        for(j = 0;j < n; j++)
            scanf("%d", s->max[i] + j);
    printf("Enter allocation matrix: ");
    for(i = 0; i < m; i++)
        for(j = 0; j < n; j++) {
            scanf("%d", s->allocation[i] + j);
        }
    printf("\n");
    
    /* Calcuate the need matrix */
    for(i = 0; i < m; i++)
        for(j = 0; j < n; j++)
            s->need[i][j] = s->max[i][j]-s->allocation[i][j];
    
    /* Calcuate the availability vector */
    for(j = 0; j < n; j++) {
        int sum = 0;
        for(i = 0; i < m; i++)
            sum += s->allocation[i][j];
        s->available[j] = s->resource[j] - sum;
    }
    
    /* Output need matrix and availability vector */
    printf("Need matrix:\n");
    for(i = 0; i < n; i++)
        printf("R%d ", i+1);
    printf("\n");
    for(i = 0; i < m; i++) {
        for(j = 0; j < n; j++)
            printf("%d  ",s->need[i][j]);
        printf("\n");
    }
    printf("Availability vector:\n");
    for(i = 0; i < n; i++)
        printf("R%d ", i+1);
    printf("\n");
    for(j = 0; j < n; j++)
        printf("%d  ",s->available[j]);
    printf("\n");
    
    /* If initial state is unsafe then terminate with error */
    
    /* Seed the random number generator */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    
    /* Create m threads */
    pthread_t tid[m];
    for (i = 0; i < m; i++)
        pthread_create(tid + i, NULL, process_thread, (void *) (long) i);
    
    /* Wait for threads to finish */
    pthread_exit(0);
    
    /* Free state memory */
    free_state(s);
}

