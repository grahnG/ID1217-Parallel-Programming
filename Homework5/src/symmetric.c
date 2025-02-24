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
        int v = rand() % 100;
    }
}
