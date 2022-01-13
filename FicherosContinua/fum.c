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

        pthread_mutex_lock(&mfum);            /* acceder al buffer */
        while (ingrediente != -1)             /*si buffer full */
            pthread_cond_wait(&vacia, &mfum); /*se bloquea */
        ingrediente = rand() % 3;
        printf("agente poniendo en la mesa para  %s \n", ing2str[ingrediente]);
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
        pthread_cond_signal(&vacia); /* huecos en buffer */
        pthread_mutex_unlock(&mfum);
        smoke(i);
    }
}
int main()
{
    int i;
    ingrediente = -1;

    pthread_mutex_init(&mfum, NULL);
    pthread_cond_init(&vacia, NULL);
    pthread_cond_init(&llena, NULL);

    //Creamos los hilos de lectores y escritores
    for (i = 0; i < NUM_FUM; i++)
        pthread_create(&fum[i], NULL, Fumador, (void *)i);
    for (i = 0; i < NUM_AGE; i++)
        pthread_create(&ag[i], NULL, Agente, NULL);

    for (i = 0; i < NUM_FUM; i++)
        pthread_join(fum[i], NULL);
    for (i = 0; i < NUM_AGE; i++)
        pthread_join(ag[i], NULL);

    pthread_mutex_destroy(&mfum);
    pthread_cond_destroy(&vacia);
    pthread_cond_destroy(&llena);

    return 0;
}