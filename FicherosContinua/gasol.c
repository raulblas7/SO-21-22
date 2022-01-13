#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define DEFAULTN 16
#define DEFAULTSUP 2

char *progname;
pthread_mutex_t m;
pthread_cond_t freesup;

int ticket = 0;
int turn = 0;
int nsup = DEFAULTSUP;
int *supplies;
#define FREE 0
#define BUSY 1

typedef struct
{
	int id;
	int money;
} tharg;

void init_thargs(tharg *thargs, int n)
{
	int i;
	srandom((unsigned int)time(NULL));

	for (i = 0; i < n; i++)
	{
		thargs[i].id = i;
		thargs[i].money = random() % 60;
	}
}

int parse_int_or_die(void)
{
	int n;
	char *endptr;

	if (!optarg || *optarg == '\0') {
		usage();
		exit(EXIT_FAILURE);
	}

	n = strtol(optarg, &endptr, 10);
	if (*endptr != '\0') {
		usage();
		exit(EXIT_FAILURE);
	}
	return n;
}

int free_supply(int *sup)
{
	int i = 0;
	int isfree = 0;

	while ((i < nsup) && (supplies[i] == BUSY))
		i++;

	if (i < nsup) {
		*sup = i;
		isfree = 1;
	}

	return isfree;
}

// < declaraci´on de variables globales >
void *Cliente(void *arg)
{
	tharg* t = (tharg*)arg;
	// < declaraci´on de variables locales >
	// Comprobar que se cumplen los requisitos para repostar
	pthread_mutex_lock(&m);
	while ()
		pthread_cond_wait(freesup, &m);
	ServirCombustible(surtidor, dinero);
	pthread_cond_signal(&freesup);
	pthread_mutex_unlock(&m);
	// Acciones de salida
}

void *Surtidor()
{
}

void usage()
{
	printf("%s Num_de_salvajes\n", progname);
}

typedef struct
{
	int id;
	int money;
} tharg;

int main(int argc, char *argv[])
{
	char c;
	int i;
	int n = DEFAULTN;
	tharg *thargs;
	pthread_t *tids;

	progname = argv[0];

	while ((c = getopt(argc, argv, "n:s:h")) != -1)
	{

		switch (c)
		{
		case 'n':
			n = parse_int_or_die();
			break;
		case 's':
			nsup = parse_int_or_die();
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}

	supplies = malloc(nsup * sizeof(int));
	if (!supplies)
	{
		perror("supplies malloc");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < nsup; i++)
	{
		supplies[i] = FREE;
	}

	thargs = malloc(n * sizeof(tharg));
	if (thargs == NULL)
	{
		perror("thargs malloc");
		exit(EXIT_FAILURE);
	}
	init_thargs(thargs, n);

	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&freesup, NULL);

	tids = malloc(n * sizeof(pthread_t));
	if (tids == NULL)
	{
		perror("tids malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n; i++)
		pthread_create(&tids[i], NULL, Cliente, (void *)&thargs[i]);

	for (i = 0; i < n; i++)
		pthread_join(tids[i], NULL);

	free(tids);
	free(thargs);
	free(supplies);

	pthread_cond_destroy(&freesup);
	pthread_mutex_destroy(&m);

	return 0;
}