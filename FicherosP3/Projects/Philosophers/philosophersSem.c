#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"
#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];
sem_t forks[NR_PHILOSOPHERS];
void init()
{
    int i;
    for (i = 0; i < NR_PHILOSOPHERS; i++)
    {
        sem_init(&forks[i], 0, 1);
    }
}

void think(int i)
{
    printf("Philosopher %d thinking... \n", i);
    sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n", i);
}

void eat(int i)
{
    printf("Philosopher %d eating... \n", i);
    sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n", i);
}

void toSleep(int i)
{
    printf("Philosopher %d sleeping... \n", i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n", i);
}

int min(int x, int y) { return x < y ? x : y; }
int max(int x, int y) { return x > y ? x : y; }

void *philosopher(void *i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    int first = min(right, left);
    int second = max(right, left);
    while (1)
    {

        think(nPhilosopher);

        /// TRY TO GRAB BOTH FORKS (right and left)
        sem_wait(&forks[first]);
        sem_wait(&forks[second]);

        eat(nPhilosopher);

        // PUT FORKS BACK ON THE TABLE
        sem_post(&forks[second]);
        sem_post(&forks[first]);

        toSleep(nPhilosopher);
    }
}

int main()
{
    init();
    unsigned long i;
    for (i = 0; i < NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void *)i);

    for (i = 0; i < NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);

    return 0;
}
