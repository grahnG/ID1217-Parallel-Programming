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
extern int matrix[MAXSIZE][MAXSIZE];

/* Main Function */
void runMatrixSumB(int size, int numWorkers, int seed) {
    pthread_t workers[MAXWORKERS];
    pthread_attr_t attr;
    long t;

    /* Read command-line arguments */
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    stripSize = size / numWorkers;

    /* Initialize matrix */
    initializeMatrix(seed);

    /* Set thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Start timer */
    double start_time = read_timer();

    /* Create worker threads and collect their results */
    ThreadResult *results[MAXWORKERS];
    for (t = 0; t < numWorkers; t++) {
        pthread_create(&workers[t], &attr, WorkerB, (void *)t);
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

    /* Print results */
    printf("MatrixSumB:\n");
    printf("The total sum is: %d\n", totalSum);
    printf("The maximum value is %d at position (%d, %d)\n", globalMax.value, globalMax.row, globalMax.col);
    printf("The minimum value is %d at position (%d, %d)\n", globalMin.value, globalMin.row, globalMin.col);
    printf("Execution time: %g sec\n", end_time - start_time);
}

/* Worker Function */
void *WorkerB(void *arg) {
    long id = (long)arg;
    int firstRow = id * stripSize;
    int lastRow = (id == numWorkers - 1) ? size - 1 : (firstRow + stripSize - 1);

    /* Allocate memory for the thread's result */
    ThreadResult *result = (ThreadResult *)malloc(sizeof(ThreadResult));
    result->localSum = 0;
    result->localMax = (MatrixElement){ .value = INT_MIN };
    result->localMin = (MatrixElement){ .value = INT_MAX };

    /* Process assigned strip */
    for (int i = firstRow; i <= lastRow; i++) {
        for (int j = 0; j < size; j++) {
            result->localSum += matrix[i][j];
            if (matrix[i][j] > result->localMax.value) {
                result->localMax = (MatrixElement){ .row = i, .col = j, .value = matrix[i][j] };
            }
            if (matrix[i][j] < result->localMin.value) {
                result->localMin = (MatrixElement){ .row = i, .col = j, .value = matrix[i][j] };
            }
        }
    }

    /* Return the result */
    return (void *)result;
}

