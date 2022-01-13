#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_FUM 3
#define NUM_AGE 1

pthread_t fum[NUM_FUM];
pthread_t ag[NUM_AGE];

sem_t age;
//sem_t sfum[NUM_FUM];
sem_t m;
sem_t tab;

int ingrediente;

const char *ing2str[] = {
    "TABACO",
    "CERILLAS",
    "PAPEL"};

void smoke(int ing0)
{
    printf("fumador con infinito %s fumando\n", ing2str[ing0]);
    sleep(rand() % 5);
}

void *Agente(void *arg)
{

    while (1)
    {

        sem_wait(&age);
        ingrediente = rand() % 3;
        printf("agente poniendo en la mesa para  %s \n", ing2str[ingrediente]);
        sem_post(&tab);
    }
}

void *Fumador(void *arg)
{
    int i = (int)arg;

    while (1)
    {
        
        sem_wait(&m);
        sem_wait(&tab);
        printf("fumador con %s accediendo a la mesa\n", ing2str[i]);
        ingrediente = -1;
        sem_post(&age);
        sem_post(&m);
        smoke(i);
    }
}
int main()
{
    int i;
    ingrediente = -1;

    sem_init(&age, 0, 1);
    sem_init(&tab, 0, 0);
    sem_init(&m, 0, 1);
    // sem_init(&fum[0], 0, 0);
    // sem_init(&fum[1], 0, 0);
    // sem_init(&fum[2], 0, 0);

    //Creamos los hilos de lectores y escritores
    for (i = 0; i < NUM_FUM; i++)
        pthread_create(&fum[i], NULL, Fumador, (void *)i);
    for (i = 0; i < NUM_AGE; i++)
        pthread_create(&ag[i], NULL, Agente, NULL);

    for (i = 0; i < NUM_FUM; i++)
        pthread_join(fum[i], NULL);
    for (i = 0; i < NUM_AGE; i++)
        pthread_join(ag[i], NULL);

    return 0;
}