#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */
#define RUNS 5  // Number of times each configuration runs for median timing
double start_time, end_time;
double serialStart_time, serialEnd_time;
double calculateMedian(double times[], int count);

/* Benchmarking Setup */
int main() {
    int matrixSizes[] = {100, 1000, 10000}; // Different matrix sizes to test
    int threadCounts[] = {1, 2, 4, 8}; // Different thread configurations
    int numSizes = sizeof(matrixSizes) / sizeof(matrixSizes[0]);
    int numThreads = sizeof(threadCounts) / sizeof(threadCounts[0]);

    printf("=== Matrix Computation Benchmark ===\n");
    printf("Size                - Matrix size\n");
    printf("Speedup = Serial Execution Time / Parallel Execution Time\n");
    printf("──────────────────────────────────────────────────────────\n");

    printf("┌───────────┬───────────────────────┬───────────────────────┬───────────────────────┐\n");
    printf("│ Threads ↓ │");
    for (int s = 0; s < numSizes; s++) {
        printf(" %8d Elements     │", matrixSizes[s]);
    }
    printf("\n");
    printf("├───────────┼");
    for (int s = 0; s < numSizes; s++) {
        printf("───────────────────────┼");
    }
    printf("\n");

    for (int t = 0; t < numThreads; t++) {
        int threads = threadCounts[t];
        printf("│ %5d     │", threads);

        for (int s = 0; s < numSizes; s++) {
            int size = matrixSizes[s];
            double serialTimes[RUNS], parallelTimes[RUNS];

            // Allocate and initialize the matrix
            int **matrix = (int **)malloc(size * sizeof(int *));
            for (int i = 0; i < size; i++) {
                matrix[i] = (int *)malloc(size * sizeof(int));
                for (int j = 0; j < size; j++) {
                    matrix[i][j] = rand() % 99;
                }
            }

            // Variables for min/max calculations
            int MaxRow, MaxColumn, MinRow, MinColumn;

            // Measure serial execution time
            for (int r = 0; r < RUNS; r++) {
                int total = 0;
                MaxRow = 0;
                MaxColumn = 0;
                MinRow = 0;
                MinColumn = 0;

                serialStart_time = omp_get_wtime();

                for (int i = 0; i < size; i++) {
                    for (int j = 0; j < size; j++) {
                        total += matrix[i][j];
                        if (matrix[i][j] > matrix[MaxRow][MaxColumn]) {
                            MaxRow = i;
                            MaxColumn = j;
                        }
                        if (matrix[i][j] < matrix[MinRow][MinColumn]) {
                            MinRow = i;
                            MinColumn = j;
                        }
                    }
                }

                serialEnd_time = omp_get_wtime();

                serialTimes[r] = serialEnd_time - serialStart_time;
            }
            double serialMedian = calculateMedian(serialTimes, RUNS);

            // Measure parallel execution time
            omp_set_num_threads(threads);
            for (int r = 0; r < RUNS; r++) {
                int total = 0;
                int globalMaxRow = 0;
                int globalMaxColumn = 0;
                int globalMinRow = 0;
                int globalMinColumn = 0;

                start_time = omp_get_wtime();

                #pragma omp parallel shared(globalMaxRow, globalMaxColumn, globalMinRow, globalMinColumn)
                {
                    int localMaxRow = 0;
                    int localMaxColumn = 0;
                    int localMinRow = 0;
                    int localMinColumn = 0; 

                    #pragma omp for reduction(+:total)
                    for (int i = 0; i < size; i++) {
                        for (int j = 0; j < size; j++) {
                            total += matrix[i][j];
                            if (matrix[i][j] > matrix[localMaxRow][localMaxColumn]) {
                                localMaxRow = i;
                                localMaxColumn = j;
                            }
                            if (matrix[i][j] < matrix[localMinRow][localMinColumn]) {
                                localMinRow = i;
                                localMinColumn = j;
                            }
                        }
                    }

                    #pragma omp critical
                    {
                        if (matrix[localMaxRow][localMaxColumn] > matrix[globalMaxRow][globalMaxColumn]) {
                            globalMaxRow = localMaxRow;
                            globalMaxColumn = localMaxColumn;
                        }
                        if (matrix[localMinRow][localMinColumn] < matrix[globalMinRow][globalMinColumn]) {
                            globalMinRow = localMinRow;
                            globalMinColumn = localMinColumn;
                        }
                    }
                }

                end_time = omp_get_wtime();

                parallelTimes[r] = end_time - start_time;
            }
            double parallelMedian = calculateMedian(parallelTimes, RUNS);
            double speedup = (parallelMedian > 0) ? serialMedian / parallelMedian : 0;

            // Ensure consistent formatting for time and speedup
            printf(" %8.6f s : %8.6f │", parallelMedian, speedup);

            // Free memory
            for (int i = 0; i < size; i++) {
                free(matrix[i]);
            }
            free(matrix);
        }
        printf("\n");
        if (t < numThreads - 1) {
            printf("├───────────┼");
            for (int s = 0; s < numSizes; s++) {
                printf("───────────────────────┼");
            }
            printf("\n");
        }
    }
    printf("└───────────┴");
    for (int s = 0; s < numSizes; s++) {
        printf("───────────────────────┴");
    }
    printf("\n");

    return 0;
}

/* Function to compute median execution time */
double calculateMedian(double times[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (times[j] > times[j + 1]) {
                double temp = times[j];
                times[j] = times[j + 1];
                times[j + 1] = temp;
            }
        }
    }
    return (count % 2 == 0) ? (times[count / 2 - 1] + times[count / 2]) / 2.0 : times[count / 2];
}



