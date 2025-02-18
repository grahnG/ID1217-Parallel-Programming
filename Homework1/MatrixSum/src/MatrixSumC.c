#include "MatrixSum.h"

/* Struct to store a matrix element's value and position */
typedef struct {
    int row;
    int col;
    int value;
} MatrixElement;

/* Struct to store thread-local results */
typedef struct {
    int localSum;
    MatrixElement localMax;
    MatrixElement localMin;
} ThreadResult;

/* Global Variables (Declared in main.c, referenced here) */
extern int size;
extern int numWorkers;
extern int stripSize;
extern int matrix[MAXSIZE][MAXSIZE];    /* Matrix */
int nextRow = 0;                  /* Shared counter for "bag of tasks" */
pthread_mutex_t rowLock;          /* Mutex Lock for shared counter */

/* Main Function */
void runMatrixSumC(int size, int numWorkers, int seed) {
    pthread_t workers[MAXWORKERS];
    pthread_attr_t attr;
    long t;

    /* Read command-line arguments */
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    stripSize = size / numWorkers;

    /* Initialize matrix and mutex */
    initializeMatrix(seed);
    pthread_mutex_init(&rowLock, NULL);

    /* Set thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Start timer */
    double start_time = read_timer();

    /* Create worker threads and collect their results */
    ThreadResult *results[MAXWORKERS];
    for (t = 0; t < numWorkers; t++) {
        pthread_create(&workers[t], &attr, WorkerC, (void *)t);
    }

    /* Aggregate results */
    int totalSum = 0;
    MatrixElement globalMax = { .value = INT_MIN };
    MatrixElement globalMin = { .value = INT_MAX };

    for (t = 0; t < numWorkers; t++) {
        ThreadResult *result;
        pthread_join(workers[t], (void **)&result);

        totalSum += result->localSum;

        if (result->localMax.value > globalMax.value) {
            globalMax = result->localMax;
        }
        if (result->localMin.value < globalMin.value) {
            globalMin = result->localMin;
        }

        /* Free the memory allocated by the thread */
        free(result);
    }

    /* Stop timer */
    double end_time = read_timer();

    /* Destroy the mutex */
    pthread_mutex_destroy(&rowLock);

    /* Print results */
    printf("MatrixSumC:\n");
    printf("The total sum is: %d\n", totalSum);
    printf("The maximum value is %d at position (%d, %d)\n", globalMax.value, globalMax.row, globalMax.col);
    printf("The minimum value is %d at position (%d, %d)\n", globalMin.value, globalMin.row, globalMin.col);
    printf("Execution time: %g sec\n", end_time - start_time);
}

/* Worker Function */
void *WorkerC(void *arg) {
    int row;
    long id = (long)arg;

    /* Allocate memory for the thread's result */
    ThreadResult *result = (ThreadResult *)malloc(sizeof(ThreadResult));
    result->localSum = 0;
    result->localMax = (MatrixElement){ .value = INT_MIN };
    result->localMin = (MatrixElement){ .value = INT_MAX };

    while (1) {
        /* Critical section: Get a row from the shared counter */
        pthread_mutex_lock(&rowLock);
        row = nextRow;
        nextRow++;
        pthread_mutex_unlock(&rowLock);

        /* Check if all rows are processed */
        if (row >= size) {
            break;
        }

        for (int j = 0; j < size; j++) {
            result->localSum += matrix[row][j];
            if (matrix[row][j] > result->localMax.value) {
                result->localMax = (MatrixElement){ .row = row, .col = j, .value = matrix[row][j] };
            }
            if (matrix[row][j] < result->localMin.value) {
                result->localMin = (MatrixElement){ .row = row, .col = j, .value = matrix[row][j] };
            }
        }
    }

    /* Return the result */
    return (void *)result;
}