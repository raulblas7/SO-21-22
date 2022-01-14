#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
sem_t s;
int total_sum = 0;
void *partial_sum(void *arg)
{
  int j = 0;
  int ni = ((int *)arg)[0];
  int nf = ((int *)arg)[1];

  sem_wait(&s);
  for (j = ni; j <= nf; j++)
  {
    total_sum = total_sum + j;
  }
  sem_post(&s);
  pthread_exit(0);
}

int main(void)
{
  pthread_t th1, th2;
  int num1[2] = {1, 4999};
  int num2[2] = {5000, 10000};
  sem_init(&s, 0, 1);
  /* Two threads are created */
  pthread_create(&th1, NULL, partial_sum, (void *)num1);
  pthread_create(&th2, NULL, partial_sum, (void *)num2);

  /* the main thread waits until both threads complete */
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  printf("total_sum=%d and it should be 50005000\n", total_sum);
  sem_destroy(&s);
  return 0;
}
