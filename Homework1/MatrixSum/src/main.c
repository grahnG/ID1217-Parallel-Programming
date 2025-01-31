#include "MatrixSum.h"

/* Define global variables */
int size;
int numWorkers;
int stripSize;
int matrix[MAXSIZE][MAXSIZE];

void initializeMatrix(int seed) {
    if (seed >= 0) {
        srand(seed); // Use the provided seed for reproducibility
    } else {
        srand(time(NULL)); // Use the current time for randomness
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 100; /* Random values [0, 99] */
        }
    }
}

double read_timer() {
    static struct timeval start;
    static int initialized = 0;
    struct timeval end;

    if (!initialized) {
        gettimeofday(&start, NULL);
        initialized = 1;
    }

    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[]) {
    // Ensure correct usage
    if (argc < 3) {
        printf("Usage: %s <matrix_size> <num_workers> [seed]\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    size = atoi(argv[1]);
    numWorkers = atoi(argv[2]);
    int seed = (argc > 3) ? atoi(argv[3]) : -1; // Default seed is -1 (random)

    // Print configuration
    printf("\nRunning all implementations with size=%d, numWorkers=%d, seed=%d\n\n", size, numWorkers, seed);

    // Run MatrixSumA
    printf("\n--- Running MatrixSumA ---\n");
    runMatrixSumA(size, numWorkers, seed);

    // Run MatrixSumB
    printf("\n--- Running MatrixSumB ---\n");
    runMatrixSumB(size, numWorkers, seed);

    // Run MatrixSumC
    printf("\n--- Running MatrixSumC ---\n");
    runMatrixSumC(size, numWorkers, seed);

    return 0;
}

