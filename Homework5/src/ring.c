#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <limits.h>
#include <string.h>

#define STARTER 0

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

    int prev_rank = (rank - 1 + size) % size; // Previous process in the ring
    int next_rank = (rank + 1) % size; // Next process in the ring

    double start_time, end_time;
    if (rank == STARTER) {
        start_time = read_timer();
    }

    for (int i = 0; i < numberOfRounds; i++) {
        int my_value = rand() % 100;
        for (int k = 0; k < 1000; k++) { volatile int x = k; }

        int min_max_values[2]; // [0] -> max, [1] -> min

        // **Round 1: Compute min and max in a pipeline**
        if (rank == STARTER) {
            min_max_values[0] = my_value; // Max
            min_max_values[1] = my_value; // Min

            if (!silent_mode) {
                printf("\n[STARTER] Round %d -> Starting ring with value: %d\n", i + 1, my_value);
            }

            // STARTER sends initial min/max value to the next process
            MPI_Send(min_max_values, 2, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        }

        // Every other process receives first, then updates min/max
        MPI_Recv(min_max_values, 2, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Update min/max based on my_value
        if (my_value > min_max_values[0]) min_max_values[0] = my_value; // Max
        if (my_value < min_max_values[1]) min_max_values[1] = my_value; // Min

        // Forward updated values to the next process
        MPI_Send(min_max_values, 2, MPI_INT, next_rank, 0, MPI_COMM_WORLD);

        if (!silent_mode) {
            printf("[Process %d] Round %d -> My value: %d | Max so far: %d | Min so far: %d\n",
                rank, i + 1, my_value, min_max_values[0], min_max_values[1]);
        }
    

        // **Round 2: Distribute final min/max across the ring**
        MPI_Recv(min_max_values, 2, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Each process receives final min/max and passes it to next
        if (rank != STARTER) {
            MPI_Send(min_max_values, 2, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        }

        if (!silent_mode) {
            printf("[Process %d] Round %d -> Received final min/max | Max: %d | Min: %d\n",
                rank, i + 1, min_max_values[0], min_max_values[1]);
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


