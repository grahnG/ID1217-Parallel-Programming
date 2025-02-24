#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <limits.h>


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
    int tillf, min, max;
    int totalTime = 0;
    int buffer[size];
    

    if (rank == STARTER) {
        start_time = read_timer();
    }

    for (int i = 0; i < numberOfRounds; i++) {
        /* Initialize seed and get the random number */
        int value = rand() % 100;
        min = INT_MAX;
        max = INT_MIN;
    
        MPI_Barrier(MPI_COMM_WORLD);
        printf("thread number: %d\n value: %d\n\n",rank, value);

        MPI_Gather(&value, 1, MPI_INT, &buffer[0], 1, MPI_INT, STARTER,MPI_COMM_WORLD);

        if(rank == STARTER){
            for(int j = 0; j<size; j++){
                if(buffer[j] > max) max = buffer[j];
                if(buffer[j] < min) min = buffer[j];
            }
            printf("Extreme values ->\n MIN: %d \n MAX \n", min, max); 

            MPI_Bcast(&min, 1, MPI_INT, STARTER, MPI_COMM_WORLD);
            MPI_Bcast(&max, 1, MPI_INT, STARTER, MPI_COMM_WORLD);

        }
        MPI_Barrier(MPI_COMM_WORLD);
        printf("thread number: %d -> min: %d",rank, min);
        MPI_Barrier(MPI_COMM_WORLD);
        printf("thread number: %d -> max: %d",rank, max);
        if(rank == STARTER){
            end_time = read_timer();
        }

        totalTime = totalTime + (end_time - start_time);
    }


    MPI_Finalize();

    int averageTime = totalTime/numberOfRounds;
        
    return 0;
}