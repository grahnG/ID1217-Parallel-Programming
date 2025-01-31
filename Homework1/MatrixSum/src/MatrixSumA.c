#include "MatrixSum.h"

/* Struct to store a matrix element's value and position */
typedef struct {
    int row;
    int col;
    int value;
} MatrixElement;

/* Global Variables (declared in main.c, referenced here) */
extern int size;
extern int numWorkers;
extern int stripSize;
extern int matrix[MAXSIZE][MAXSIZE];

/* Variables specific to MatrixSumA */
pthread_mutex_t barrierLock; /* Mutex lock for the barrier */
pthread_cond_t barrierCond;   /* Condition variable for the barrier */
int numArrived = 0;           /* Number of threads that arrived at the barrier */
int partialSums[MAXWORKERS];  /* Partial sums */
MatrixElement maxPerWorker[MAXWORKERS]; /* Per-worker max elements */
MatrixElement minPerWorker[MAXWORKERS]; /* Per-worker min elements */

/* Function Prototypes */
void Barrier();

/* Main Function */
void runMatrixSumA(int size, int numWorkers, int seed) {
    pthread_t workers[MAXWORKERS];
    pthread_attr_t attr;
    long t;

    /* Ensure size and numWorkers are within limits */
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    stripSize = size / numWorkers; // Defined in main.c

    /* Initialize matrix */
    initializeMatrix(seed);

    /* Set thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Start timer */
    double start_time = read_timer();

    /* Create worker threads */
    for (t = 0; t < numWorkers; t++) {
        pthread_create(&workers[t], &attr, WorkerA, (void *)t);
    }

    /* Wait for all threads to complete */
    for (t = 0; t < numWorkers; t++) {
        pthread_join(workers[t], NULL);
    }

    /* Stop timer */
    double end_time = read_timer();

    /* Print execution time */
    printf("MatrixSumA Execution time: %g sec\n", end_time - start_time);
}

/* Worker Function */
void *WorkerA(void *arg) {
    long id = (long)arg;
    int firstRow = id * stripSize;
    int lastRow = (id == numWorkers - 1) ? size - 1 : (firstRow + stripSize - 1);

    /* Local variables */
    int localSum = 0;
    MatrixElement localMax = { .value = INT_MIN };
    MatrixElement localMin = { .value = INT_MAX };

    /* Process assigned strip */
    for (int i = firstRow; i <= lastRow; i++) {
        for (int j = 0; j < size; j++) {
            localSum += matrix[i][j];
            if (matrix[i][j] > localMax.value) {
                localMax = (MatrixElement){ .row = i, .col = j, .value = matrix[i][j] };
            }
            if (matrix[i][j] < localMin.value) {
                localMin = (MatrixElement){ .row = i, .col = j, .value = matrix[i][j] };
            }
        }
    }

    /* Save results */
    partialSums[id] = localSum;
    maxPerWorker[id] = localMax;
    minPerWorker[id] = localMin;

    /* Synchronize using the barrier */
    Barrier();

    /* Worker 0 aggregates and prints results */
    if (id == 0) {
        int totalSum = 0;
        MatrixElement globalMax = { .value = INT_MIN };
        MatrixElement globalMin = { .value = INT_MAX };

        for (int i = 0; i < numWorkers; i++) {
            totalSum += partialSums[i];

            if (maxPerWorker[i].value > globalMax.value) {
                globalMax = maxPerWorker[i];
            }

            if (minPerWorker[i].value < globalMin.value) {
                globalMin = minPerWorker[i];
            }
        }

        /* Print results */
        printf("The total sum is: %d\n", totalSum);
        printf("The maximum value is %d at position (%d, %d)\n", globalMax.value, globalMax.row, globalMax.col);
        printf("The minimum value is %d at position (%d, %d)\n", globalMin.value, globalMin.row, globalMin.col);
    }

    return NULL;
}

/* Reusable Barrier Implementation */
void Barrier() {
    pthread_mutex_lock(&barrierLock);
    numArrived++;
    if (numArrived == numWorkers) {
        numArrived = 0;
        pthread_cond_broadcast(&barrierCond);
    } else {
        pthread_cond_wait(&barrierCond, &barrierLock);
    }
    pthread_mutex_unlock(&barrierLock);
}

