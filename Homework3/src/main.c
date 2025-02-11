
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>  
#include <semaphore.h>

#define MAXWOMEN 10 /* maximum number of women */
#define MAXMEN 10 /* maximum number of men */
#define MAXVISITS 50 /* maximum number of visits */

sem_t *bathroom_access;
sem_t *women_queue;
sem_t *men_queue;

int current_women_in_bath = 0;
int current_men_in_bath = 0;
int waiting_women = 0;
int waiting_men = 0;

bool women_exit_phase;
bool men_exit_phase;

int numberOfWomen = 0;
int numberOfMen = 0;
int numberOfVisits = 0;

void *Women(void *arg)
{
    long id = (long) arg;
    int i;
    for(i = 0; i < numberOfVisits; i++){

        usleep((rand() % 4000) * 1000);

        sem_wait(bathroom_access);


        if(current_men_in_bath > 0 || men_exit_phase){

        }
    }
}

void *Men(void *arg) 
{

}

int main ()
{
    
}