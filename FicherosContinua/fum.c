#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_FUM 3
#define NUM_AGE 1

pthread_t fum[NUM_FUM];
pthread_t ag[NUM_AGE];

pthread_cond_t llena;
pthread_cond_t vacia;
pthread_mutex_t mfum;

int ingrediente;

void *Agente(void *arg)
{
    while (1)
    {

        pthread_mutex_lock(&mfum); /* acceder al buffer */
        while (ingrediente != -1)             /*si buffer full */
            pthread_cond_wait(&vacia, &mfum); /*se bloquea */
        printf("POngo las cosas \n");
        ingrediente = rand() % 3;
        pthread_cond_broadcast(&llena); /* buffer con elementos */
        pthread_mutex_unlock(&mfum);
    }
}

void *Fumador(void *arg)
{
    int i = (int)arg;

    while (1)
    {

        pthread_mutex_lock(&mfum);            /* acceder al buffer */
        while (ingrediente != i)              /* si buffer empty */
            pthread_cond_wait(&llena, &mfum); /* se bloquea */
        ingrediente = -1;
        printf("Fume %d \n", i);
        pthread_cond_signal(&vacia); /* huecos en buffer */
        pthread_mutex_unlock(&mfum);
    }
}
int main()
{
    int i;
    ingrediente = -1;
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