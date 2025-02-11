
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
    long id = (int)arg;
    for (int i = 0; i < numberOfVisits; i++) 
    {
        /* Do different things before entering*/
        usleep((rand() % 4000) * 1000);

        /* Check if the bathroom is occupied; if so, wait */
        sem_p(bathroom_access);
        if (current_men_in_bath > 0 || women_exit_phase) 
        {
            waiting_women++;
            sem_v(bathroom_access);
            sem_p(women_queue);
        }
        current_women_in_bath++;
        if (waiting_women > 0)
        {
            waiting_women--;
            sem_v(women_queue);
        }
        else
        {
            sem_v(bathroom_access);
        }
    }
}

void *Men(void *arg) 
{

}

int main ()
{

}