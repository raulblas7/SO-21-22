#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "semaphore.h"
#define M 10

pthread_t coc;
sem_t servir;
sem_t cocinar;
sem_t m;
int platos;
char *progname;

void putServingsInPot(int num)
{
    platos += num;
}

void *Cocinero()
{
    while (1)
    {

        sem_wait(&cocinar);
        putServingsInPot(M);
        printf("Llenando caldero   %d \n", platos);
        sem_post(&servir);
    }
}

void *Salvajes(void *arg)
{
    int i = (int)arg;
    while (1)
    {

        sem_wait(&m);
        printf("Salvaje %d sirviendo ración del caldero %d \n", i, platos);
        if (platos == 0)
        {
            sem_post(&cocinar);
            sem_wait(&servir);
        }
        platos--;
        sem_post(&m);
        printf("Salvaje %d comiendo \n", i);
        sleep(rand() % 5);
    }
}

void usage()
{
    printf("%s Num_de_salvajes\n", progname);
}

int main(int argc, char *argv[])
{
    int i;
    platos = 0;

    char *endptr;
    progname = argv[0];

    int n;
    n = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0')
    {
        usage();
        exit(EXIT_FAILURE);
    }

    pthread_t *tid = malloc(n * sizeof(pthread_t));
    if (tid == NULL)
    {
        perror("malloc tid");
        exit(EXIT_FAILURE);
    }
    memset(tid, 0, n * sizeof(pthread_t));

    sem_init(&cocinar, 0, 0);
    sem_init(&servir, 0, 0);
    sem_init(&m, 0, 1);

    //Creamos los hilos de lectores y escritores
    for (i = 0; i < n; i++)
        pthread_create(&tid[i], NULL, Salvajes, (void *)i);

    pthread_create(&coc, NULL, Cocinero, NULL);

    for (i = 0; i < n; i++)
        pthread_join(tid[i], NULL);

    pthread_join(coc, NULL);

    return 0;
}