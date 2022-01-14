#include <stdlib.h>
#include <stdio.h>
#include "pthread.h"
#define N_PARADAS 5		// número de paradas de la ruta
#define EN_RUTA 0		// autobús en ruta
#define EN_PARADA 1		// autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4		// numero de usuarios
// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0;   // ocupantes que tiene el autobús
// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};
// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};

// Otras definiciones globales (comunicación y sincronización)
pthread_mutex_t bus;
pthread_mutex_t subir;
pthread_mutex_t bajar;

pthread_cond_t parado;
pthread_cond_t movi;

void *thread_autobus(void *args)
{
	while (1)
	{
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}
void *thread_usuario(void *arg)
{
	int id_usuario = (int)arg;
	// obtener el id del usario
	while (n_ocupantes < MAX_USUARIOS)
	{
		int a = rand() % N_PARADAS, b;
		do
		{
			b = rand() % N_PARADAS;
		} while (a == b);
		Usuario(id_usuario, a, b);
	}
}
void Usuario(int id_usuario, int origen, int destino)
{
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
}
int main(int argc, char *argv[])
{

	int i;
	int n = USUARIOS;
	pthread_t *tids;
	pthread_t bus;

	tids = malloc(n * sizeof(pthread_t));
	if (tids == NULL)
	{
		perror("tids malloc");
		exit(EXIT_FAILURE);
	}

	pthread_cond_init(&movi, NULL);
	pthread_cond_init(&parado, NULL);

	pthread_create(&bus, NULL, thread_autobus, NULL);
	for (i = 0; i < n; i++)
		pthread_create(&tids[i], NULL, thread_usuario, (void *)i);

	for (i = 0; i < n; i++)
		pthread_join(tids[i], NULL);
	pthread_join(&bus, NULL);

	free(tids);

	return 0;
}
void Autobus_En_Parada()
{
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
quieran bajar y/o subir la parada actual. Después se pone en marcha */
	estado = EN_PARADA;
	printf("Personas que quieren subir %d \n", esperando_parada[parada_actual]);
	printf("Personas que quieren bajar %d \n", esperando_bajar[parada_actual]);
	pthread_mutex_lock(&bus);
	pthread_cond_broadcast(&movi);
	while (esperando_bajar[parada_actual] !=0 || esperando_parada[parada_actual]!=0)
	{
		printf("Autobus Esperando\n");
		pthread_cond_wait(&parado, &bus);
	}
	pthread_mutex_unlock(&bus);
	estado = EN_RUTA;
}
void Conducir_Hasta_Siguiente_Parada()
{
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	sleep(3);
	printf("Personas en bus %d \n", n_ocupantes);
	parada_actual = (parada_actual + 1) % N_PARADAS;
}
void Subir_Autobus(int id_usuario, int origen)
{
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
proporcionar información de depuración */
	pthread_mutex_lock(&subir);
	printf("El usuario %d se quiere subir en esta parada %d \n", id_usuario, origen);
	esperando_parada[origen] = esperando_parada[origen] + 1;
	while (parada_actual != origen)
	{
		pthread_cond_wait(&movi, &subir);
	}
	n_ocupantes++;
	esperando_parada[origen] = esperando_parada[origen] - 1;

	printf("El usuario %d ha subido al autobus\n", id_usuario);
	pthread_cond_signal(&parado);
	pthread_mutex_unlock(&subir);
}
void Bajar_Autobus(int id_usuario, int destino)
{
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
proporcionar información de depuración */
	pthread_mutex_lock(&bajar);
	printf("El usuario %d se quiere bajar en esta parada %d \n", id_usuario, destino);

	esperando_bajar[destino] = esperando_bajar[destino] + 1;
	while (parada_actual != destino)
	{
		pthread_cond_wait(&movi, &bajar);
	}
	n_ocupantes--;
	esperando_bajar[destino] = esperando_bajar[destino] -1;

	printf("El usuario %d ha bajado del autobus\n", id_usuario);
	pthread_cond_signal(&parado);
	pthread_mutex_unlock(&bajar);
}
