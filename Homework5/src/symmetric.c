#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <limits.h>
#include <string.h>

#define STARTER 0

// Timer function
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;

    if (!initialized) {
        gettimeofday(&start, NULL);
        initialized = true;
    }

    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[])
{
    int numberOfRounds = 3;  
    int silent_mode = 0;  

    if (argc > 1) numberOfRounds = atoi(argv[1]);
    if (argc > 2 && strcmp(argv[2], "--silent") == 0) silent_mode = 1;

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) * rank);

    double start_time, end_time;
    if (rank == STARTER) {
        start_time = read_timer();
    }

    for (int i = 0; i < numberOfRounds; i++) {
        // Each process generates a random number
        int my_value = rand() % 100;

        // Array to store values from all processes
        int all_values[size];

        // Each process sends its value to all others and receives everyone's values
        MPI_Allgather(&my_value, 1, MPI_INT, all_values, 1, MPI_INT, MPI_COMM_WORLD);

        // Each process computes min/max independently
        int min_value = my_value;
        int max_value = my_value;

        for (int j = 0; j < size; j++) {
            if (all_values[j] > max_value) max_value = all_values[j];
            if (all_values[j] < min_value) min_value = all_values[j];
        }

        // Print results for each process
        if (!silent_mode) {
            printf("[Process %d] Round %d -> My value: %d | Max: %d | Min: %d\n",
               rank, i + 1, my_value, max_value, min_value);
            }
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

