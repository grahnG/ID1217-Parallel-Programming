#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define THRESHOLD 5000  // Threshold for switching to serial sort
#define RUNS 5  // Number of runs per configuration

/* Swap function */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* Median-of-Three Pivot Selection */
int medianOfThree(int left, int right, int *array) {
    int mid = left + (right - left) / 2;
    if (array[left] > array[mid]) swap(&array[left], &array[mid]);
    if (array[left] > array[right]) swap(&array[left], &array[right]);
    if (array[mid] > array[right]) swap(&array[mid], &array[right]);
    return mid;
}

/* Partition function */
int partition(int left, int right, int *array) {
    int pivotIndex = medianOfThree(left, right, array);
    swap(&array[pivotIndex], &array[right]);
    int pivot = array[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (array[j] < pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[right]);
    return i + 1;
}

/* Serial Quicksort */
void serialQuicksort(int left, int right, int *array) {
    if (left < right) {
        int pivotIndex = partition(left, right, array);
        serialQuicksort(left, pivotIndex - 1, array);
        serialQuicksort(pivotIndex + 1, right, array);
    }
}

/* Parallel Quicksort */
void parallelQuicksort(int left, int right, int *array) {
    if (left < right) {
        int pivotIndex = partition(left, right, array);

        if ((right - left) > THRESHOLD) {
            #pragma omp task   // Spawn task only for left partition
            parallelQuicksort(left, pivotIndex - 1, array);

            // Main thread continues with right partition
            parallelQuicksort(pivotIndex + 1, right, array);

            #pragma omp taskwait  // Wait only for left side to finish
        } else {
            serialQuicksort(left, right, array);
        }
    }
}

/* Function to calculate median of an array */
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

int main() {
    int arraySizes[] = {10000, 100000, 1000000};
    int threadCounts[] = {1, 2, 4, 8};
    int numSizes = sizeof(arraySizes) / sizeof(arraySizes[0]);
    int numThreads = sizeof(threadCounts) / sizeof(threadCounts[0]);

    printf("=== Quicksort ===\n");
    printf("Size                  - Array size\n");
    printf("-.------ s : -.------ - Elapsed time : Speedup\n");
    printf("┌───────────┬───────────────────────┬───────────────────────┬───────────────────────┐\n");
    printf("│ Size    → │");
    for (int s = 0; s < numSizes; s++) {
        printf(" %8d              │", arraySizes[s]);
    }
    printf("\n");
    printf("├─Threads ↓─┼");
    for (int s = 0; s < numSizes; s++) {
        printf("───────────────────────┼");
    }
    printf("\n");

    for (int t = 0; t < numThreads; t++) {
        int threads = threadCounts[t];
        printf("│ %5d     │", threads);

        for (int s = 0; s < numSizes; s++) {
            int size = arraySizes[s];
            double serialTimes[RUNS], parallelTimes[RUNS];

            // Allocate and initialize the array
            int *array = (int *)malloc(sizeof(int) * size);
            int *copy = (int *)malloc(sizeof(int) * size);
            if (!array || !copy) {
                printf("Memory allocation error!\n");
                return 1;
            }

            srand(time(NULL));
            for (int i = 0; i < size; i++) {
                array[i] = rand() % (size * 10);
                copy[i] = array[i];
            }

            // Measure serial quicksort for baseline
            for (int r = 0; r < RUNS; r++) {
                for (int i = 0; i < size; i++) array[i] = copy[i];
                double start = omp_get_wtime();
                serialQuicksort(0, size - 1, array);
                serialTimes[r] = omp_get_wtime() - start;
            }
            double serialMedian = calculateMedian(serialTimes, RUNS);

            // Measure parallel quicksort if not serial
            omp_set_num_threads(threads);
            for (int r = 0; r < RUNS; r++) {
                for (int i = 0; i < size; i++) array[i] = copy[i];
                double start = omp_get_wtime();
                #pragma omp parallel
                {
                    #pragma omp single nowait
                    parallelQuicksort(0, size - 1, array);
                }
                parallelTimes[r] = omp_get_wtime() - start;
            }
            double parallelMedian = calculateMedian(parallelTimes, RUNS);
            double speedup = (parallelMedian > 0) ? serialMedian / parallelMedian : 0;  // Avoid division by zero

            printf(" %8.6f s : %8.6f │", parallelMedian, speedup);
            free(array);
            free(copy);
        }
        printf("\n");
        if (t < numThreads - 1) {  // Add horizontal line between thread counts
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
