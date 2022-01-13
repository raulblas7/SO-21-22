#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define M 10

pthread_t coc;

pthread_cond_t llena;
pthread_cond_t vacia;
pthread_mutex_t mfum;

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

        pthread_mutex_lock(&mfum);            /* acceder al buffer */
        while (platos != 0)                   /*si buffer full */
            pthread_cond_wait(&vacia, &mfum); /*se bloquea */
        putServingsInPot(M);
        printf("Llenando caldero   %d \n", platos);
        pthread_cond_broadcast(&llena); /* buffer con elementos */
        pthread_mutex_unlock(&mfum);
    }
}

void *Salvajes(void *arg)
{
    int i = (int)arg;
    while (1)
    {

        pthread_mutex_lock(&mfum); /* acceder al buffer */
        while (platos == 0)
        {                                     /* si buffer empty */
            pthread_cond_signal(&vacia);      /* huecos en buffer */
            pthread_cond_wait(&llena, &mfum); /* se bloquea */
        }
        printf("Salvaje %d sirviendo ración del caldero %d \n", i, platos);
        platos--;
        pthread_mutex_unlock(&mfum);
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

    pthread_mutex_init(&mfum, NULL);
    pthread_cond_init(&vacia, NULL);
    pthread_cond_init(&llena, NULL);

    //Creamos los hilos de lectores y escritores
    for (i = 0; i < n; i++)
        pthread_create(&tid[i], NULL, Salvajes, (void *)i);

    pthread_create(&coc, NULL, Cocinero, NULL);

    for (i = 0; i < n; i++)
        pthread_join(tid[i], NULL);

    pthread_join(coc, NULL);

    pthread_mutex_destroy(&mfum);
    pthread_cond_destroy(&vacia);
    pthread_cond_destroy(&llena);

    return 0;
}