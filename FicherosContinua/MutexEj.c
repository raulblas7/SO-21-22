/// Anass Carreau
/// LECTORES Y ESCRITORES CON MUTEX Y VARIABLES DE CONDICION
#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_REA 3
#define NUM_WRI 2

pthread_t rea[NUM_REA];
pthread_t wri[NUM_WRI];

pthread_cond_t finLeer;
pthread_mutex_t mrea;
pthread_mutex_t mwri;

int lectores = 0;
int data = 0;

void* Writer() {
    while (1) {
		//Seccion critica entre lock y unlock
        pthread_mutex_lock(&mwri);
        while(lectores > 0)
            pthread_cond_wait(&finLeer, &mwri);
        printf("Escribe %d\n", data);
        data +=  2;
        pthread_mutex_unlock(&mwri);
    }
}
void* Reader() {
    while (1) {
		//Seccion critica entre lock y unlock
        pthread_mutex_lock(&mrea);
        if(lectores == 0) {
            pthread_mutex_lock(&mwri);
        }
        lectores ++;
        pthread_mutex_unlock(&mrea);
        printf("Lee %d\n", data);
        pthread_mutex_lock(&mrea);
        lectores --;
        pthread_cond_broadcast(&finLeer);
        if(lectores == 0) {
            pthread_mutex_unlock(&mwri);
        }
        pthread_mutex_unlock(&mrea);
        printf("Deja de leer\n");
    }
}

int main() {
    int i;
	//Creamos los hilos de lectores y escritores
    for(i=0; i<NUM_REA; i++) pthread_create(&rea[i], NULL, Reader, (void*)i);
    for(i=0; i<NUM_WRI; i++) pthread_create(&wri[i], NULL, Writer, (void*)i);

    for(i=0; i<NUM_REA; i++) pthread_join(rea[i],NULL);
    for(i=0; i<NUM_WRI; i++) pthread_join(wri[i],NULL);

    return 0;
}