#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>

#define MAXNUMBEROFROUNDS 5

#define STARTER 0

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

int main(int argc, char *argv[])
{
	int numberOfRounds = (argc > 1) ? atoi(argv[1]) : MAXNUMBEROFROUNDS;

    int rank;
    int size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) * rank);

    int prev_rank = (rank - 1 + size) % size; // Prevoius process in the ring
    int next_rank = (rank + 1) % size; // Next process in the ring

    double start_time;
    double end_time;
    if (rank == STARTER) {
        start_time = read_timer();
    }

    for (int i = 0; i < numberOfRounds; i++) {
        /* Initialize seed and get the random number */
        int my_value = rand() % 100;

        int min_max_values[2]; // [0] -> max value, [1] -> min value

        // Round 1: compute min and max in a pipeline
        if (rank  == STARTER)
        {
            // Initialize with STARTER'S own value
            min_max_values[0] = my_value; // Max
            min_max_values[1] = my_value; // Min

            printf("\n[STARTER] Starting ring with value: %d\n", my_value);

            // STARTER sends intial min/max value to the next process
            MPI_Send(&min_max_values, 2, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        }

        // Receive min/max from the previous process
        MPI_Recv(&min_max_values, 2, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Update min/max based on my_value
        min_max_values[0] = (my_value > min_max_values[0]) ? my_value : min_max_values[0]; // Max
        min_max_values[1] = (my_value < min_max_values[1]) ? my_value : min_max_values[1]; // Min

        // Forward updated values to the next process
        MPI_Send(&min_max_values, 2, MPI_INT, next_rank, 0, MPI_COMM_WORLD);

        printf("[P%d] My value: %d | Max so far: %d | Min so far: %d\n", rank, my_value, min_max_values[0], min_max_values[1]);

        // Round 2: Distribute final min/max across the ring
        MPI_Recv(&min_max_values, 2, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Forward to the next process (except last one)
        if (rank < size - 1)
        {
            MPI_Send(&min_max_values, 2, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        }
        printf("[P%d] Received final min/max | Max: %d | Min: %d\n", rank, min_max_values[0], min_max_values[1]);
    }

    if (rank == STARTER)
    {
        end_time = read_timer();
    }

    MPI_Finalize();

    if (rank == STARTER) 
    {
        printf("%d rounds took %g seconds\n", numberOfRounds, end_time - start_time);
    }

    return 0;
}