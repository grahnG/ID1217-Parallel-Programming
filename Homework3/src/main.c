
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>  
#include <semaphore.h>
#include <unistd.h>

#define MAXWOMEN 10 /* maximum number of women */
#define MAXMEN 10 /* maximum number of men */
#define MAXVISITS 50 /* maximum number of visits */

sem_t bathroom_access; // initialize semaphores
sem_t women_queue;
sem_t men_queue;

int current_women_in_bath = 0; //global variables
int current_men_in_bath = 0;
int waiting_women = 0;
int waiting_men = 0;

bool women_exit_phase; // ensures that men are prioritized
bool men_exit_phase; 

int numberOfWomen = 0;
int numberOfMen = 0;
int numberOfVisits = 0;


void *Women(void *arg)
{
    long id = (long)(intptr_t)arg; // local variables
    for (int i = 0; i < numberOfVisits; i++) 
    {
        /* Do different things before entering*/
        usleep((rand() % 4000) * 1000);

        /* Check if the bathroom is occupied; if so, wait */
        sem_wait(&bathroom_access); // lock access to shared variables
        if (current_men_in_bath > 0 || women_exit_phase)  // if men inside bathroom or women_exit_phase is true
        {
            waiting_women++; // women waits
            sem_post(&bathroom_access); // release the lock so other threads can access shared variables
            sem_wait(&women_queue); // woman waits for a signal to enter
        }
        /* Entering the bathroom */
        current_women_in_bath++; 
        if (waiting_women > 0) //if waiting women > 0
        {
            waiting_women--; // decrement
            sem_post(&women_queue); // and let one woman enter the bathroom
        }
        else
        {
            sem_post(&bathroom_access); // otherwise the lock is released
        }

        /* Women goes to the bathroom */
        printf("Woman %ld enters the bathroom. Visit: %d\n", id, i + 1);
        usleep((rand() % 3000) * 1000);

        /* After finished bathroom visit */
        sem_wait(&bathroom_access); 
        current_women_in_bath--; // the woman locks access and decrements itself

        /* Make sure men will enter after a women went to the bathroom */
        women_exit_phase = true;  // signal men's turn
        if (current_women_in_bath == 0) { 
            women_exit_phase = false; // if no women remain, then reset women_exit_phase
        }

        /* Prio men for fairness */
        if (current_women_in_bath == 0 && waiting_men > 0)
        {
            waiting_men--;
            sem_post(&men_queue); //if men are waiting, one is signaled to enter
        }
        else
        {
            sem_post(&bathroom_access); // otherwise bathroom is unlocked
        }
    }
    pthread_exit(NULL);
}

void *Men(void *arg)
{
    long id = (long)(intptr_t) arg;
    int i;
    for(i = 0; i < numberOfVisits; i++){
        /* Do different things before entering*/
        usleep((rand() % 4000) * 1000);

        /* Check if the bathroom is occupied; if so, wait */
        sem_wait(&bathroom_access); // lock access to shared variables
        if(current_women_in_bath > 0 || men_exit_phase) // if women inside bathroom or men_exit_pgase is true
        {
            waiting_men += 1; // men waits, increment
            sem_post(&bathroom_access); // release the lock so other threads can access shared variables
            sem_wait(&men_queue); // man waits for signal to enter
        }

        /* Entering the bathroom */
        current_men_in_bath++;
        if(waiting_men >0) //if waiting men > 0
        {
            waiting_men--; // decrement
            sem_post(&men_queue); // and let one man enter the bathroom
        } else{
            sem_post(&bathroom_access);  // otherwise the lock is released
        }

        printf( "man number %ld enters the bathroom. Visit number:  %d\n",id,  i+1);
        
        /* do something else after they've been to the bathroom*/
        usleep((rand() % 3000) * 1000);
        
        /* After finished bathroom visit */
        sem_wait(&bathroom_access); // the man locks access and decrements itself
        current_men_in_bath--;

        /* Make sure women will enter after a man went to the bathroom */
        men_exit_phase = true; // signal women's turn

        if(current_men_in_bath == 0 ) men_exit_phase = false; // if no men remain, then reset men_exit_phase

        /* Prio women for fairness */
        if(current_men_in_bath == 0 && waiting_women >0)
        {
            waiting_women--;
            sem_post(&women_queue); //if women are waiting, one is signaled to enter
        } 
        else 
        {
            sem_post(&bathroom_access); // otherwise bathroom is unlocked
        }


    }
    pthread_exit(NULL);
}


int main()
{
	while (numberOfMen < 1 || numberOfMen > MAXMEN)
	{
		printf("Enter the number of men: (max %d) ", MAXMEN);
		scanf("%d", &numberOfMen);
	}
	while (numberOfWomen <= 0 || numberOfWomen > MAXWOMEN)
	{
		printf("Enter the number of women: (max %d) ", MAXWOMEN);
		scanf("%d", &numberOfWomen);
	}
	while (numberOfVisits <= 0 || numberOfVisits > MAXVISITS)
	{
		printf("Enter the number of visits per person: (max %d) ", MAXVISITS);
		scanf("%d", &numberOfVisits);
	}
	printf("\n");
    
    sem_init(&bathroom_access, 0, 1); //initialized to 1, meaning one thread can access it at a time
    sem_init(&men_queue, 0, 0);
    sem_init(&women_queue, 0, 0);

	pthread_t threads_men[numberOfMen], threads_women[numberOfWomen];
	int i;
	for (i = 0; i < numberOfMen; i++)
		pthread_create(&threads_men[i], NULL, Men, (void *)(intptr_t)i);
	for (i = 0; i < numberOfWomen; i++)
		pthread_create(&threads_women[i], NULL, Women, (void *)(intptr_t)i);

	for (i = 0; i < numberOfMen; i++)
		pthread_join(threads_men[i], NULL);
	for (i = 0; i < numberOfWomen; i++)
		pthread_join(threads_women[i], NULL);

    sem_destroy(&bathroom_access);
    sem_destroy(&men_queue);
    sem_destroy(&women_queue);
        
    
}