#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>
#include "sync.h"



spinlock_t lock;
static mutex_t lock2;


void *worker_spin(void *arg) {

  while (1) {
	spinlock_lock(&lock);
	printf("[%lu] Hello, World!\n", pthread_self());
	sleep(1);
	spinlock_unlock(&lock);
  }

  return NULL;
}

int *memory;

void *worker_mutex(void *arg) {

  while (1) {
	mutex_lock(&lock2);
	if (memory == NULL) {
		memory = (int *)malloc(sizeof(int));
	} else {
		free(memory);
		memory = NULL;
	}
	mutex_unlock(&lock2);


//	sleep(1);
  }

  return NULL;
}



int main() {

  spinlock_init(&lock);
  mutex_init(&lock2);

  pthread_t tid1, tid2;

  pthread_create(&tid1, NULL, worker_mutex, NULL);
  pthread_create(&tid2, NULL, worker_mutex, NULL);


  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
//  printf("%d\n", i);
  return 0;
}
