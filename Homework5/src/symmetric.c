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

    double start_time;
    double end_time;
    if (rank == STARTER) {
        start_time = read_timer();
    }

    for (int i = 0; i < numberOfRounds; i++) {
        /* Initialize seed and get the random number */
        int my_value = rand() % 100;

        /* Array to store value from all processes */
        int all_values[size];

        /* Each process sends its value to all others and receive everyone's values */
        MPI_Allgather(&my_value, 1, MPI_INT, &all_values[0], 1, MPI_INT, MPI_COMM_WORLD);

        // Compute the minimum and maximum values from all collected values
        int min_value = my_value;
        int max_value = my_value;

        for (int i = 0; i < size; i++) {
            if (all_values[i] > max_value) max_value = all_values[i];
            if (all_values[i] < min_value) min_value = all_values[i];
        }

        // Print results for each process
        printf("[Process %d] My value: %d | Max: %d | Min: %d\n", rank, my_value, max_value, min_value);
    }

    if (rank == ROOT) {
        end_time = read_timer();
        printf("%d rounds took %g seconds\n", numberOfRounds, end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
