#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>

#define MAXSIZE 10000
#define MAXWORKERS 10

/* Declare global variables as extern */
extern int size;
extern int numWorkers;
extern int stripSize;
extern int matrix[MAXSIZE][MAXSIZE];

/* Function Prototypes */
void runMatrixSumA(int size, int numWorkers, int seed);
void runMatrixSumB(int size, int numWorkers, int seed);
void runMatrixSumC(int size, int numWorkers, int seed);
void initializeMatrix(int seed);
double read_timer();
void *WorkerA(void *arg);
void *WorkerB(void *arg);
void *WorkerC(void *arg);