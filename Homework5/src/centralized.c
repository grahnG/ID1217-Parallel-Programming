#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <limits.h>
#include <string.h>

#define STARTER 0

// Function to measure elapsed time
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;

    if (!initialized) {
        gettimeofday(&start, NULL);
        initialized = 1;
    }

    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[]) {
    int numberOfRounds = 3;
    int silent_mode = 0;

    if (argc > 1) numberOfRounds = atoi(argv[1]);
    if (argc > 2 && strcmp(argv[2], "--silent") == 0) silent_mode = 1;

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) * rank);  // Seed random number generator

    double start_time, end_time;
    if (rank == STARTER) {
        start_time = read_timer();
    }

    for (int i = 0; i < numberOfRounds; i++) {
        // Each process generates a random number
        int my_value = rand() % 100;
        for (int k = 0; k < 1000; k++) { volatile int x = k; }

        // Buffer to store collected values
        int all_values[size];
        int final_results[2]; // [0] -> max, [1] -> min
        int min, max;

        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes start together

        // Gather all values at STARTER process
        MPI_Gather(&my_value, 1, MPI_INT, all_values, 1, MPI_INT, STARTER, MPI_COMM_WORLD);

        if (rank == STARTER) {
            max = INT_MIN;
            min = INT_MAX;

            // Find the maximum and minimum values
            for (int j = 0; j < size; j++) {
                if (all_values[j] > max) max = all_values[j];
                if (all_values[j] < min) min = all_values[j];
            }

            final_results[0] = max;
            final_results[1] = min;

            if (!silent_mode) {
                printf("\n[STARTER] Round %d -> MIN: %d | MAX: %d\n", i + 1, min, max);
            }

            // Send results manually to each process
            for (int j = 1; j < size; j++) {
                MPI_Send(final_results, 2, MPI_INT, j, 0, MPI_COMM_WORLD);
            }

            // **STARTER prints results for all processes to ensure correct order**
            if (!silent_mode) {
            for (int p = 0; p < size; p++) {
                printf("[Process %d] Round %d -> My value: %d | Max: %d | Min: %d\n",
                       p, i + 1, all_values[p], max, min);
            }
          }
        } else {
            // Each process receives min/max from STARTER
            MPI_Recv(final_results, 2, MPI_INT, STARTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes received values before continuing
    }

    if (rank == STARTER) {
        end_time = read_timer();
        if (!silent_mode) {
            printf("\n%d rounds took %g seconds\n", numberOfRounds, end_time - start_time);
        } else {
            printf("%g\n", end_time - start_time);  // Only print execution time in silent mode
        }
    }

    MPI_Finalize();
    return 0;
}

