#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "queue.h"

void *qmonitor(void *arg) {
  queue_t *q = (queue_t *)arg;

  printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

  while (1) {
    queue_print_stats(q);
    sleep(1);
  }

  return NULL;
}

queue_t *queue_init(int max_count) {
  int err;

  queue_t *q = malloc(sizeof(queue_t));
  if (!q) {
    printf("Cannot allocate memory for a queue\n");
    abort();
  }

  q->add_avg_time = 0;
  q->get_avg_time = 0;

  // pthread_spin_init(&q->spinlock, 0);
  pthread_mutex_init(&q->mutex, NULL);
  // pthread_cond_init(&q->cond, NULL);
  // sem_init(&q->sem, 0, 1);

  q->first = NULL;
  q->last = NULL;
  q->max_count = max_count;
  q->count = 0;

  q->add_attempts = q->get_attempts = 0;
  q->add_count = q->get_count = 0;

  err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
  if (err) {
    printf("queue_init: pthread_create() failed: %s\n", strerror(err));
    abort();
  }

  return q;
}

void queue_destroy(queue_t *q) {
  pthread_cancel(q->qmonitor_tid);



  // printf("add avg time: %lf\n", q->add_avg_time / q->add_attempts);
  // printf("get avg time: %lf\n", q->get_avg_time / q->get_attempts);

  // pthread_spin_destroy(&q->spinlock);
  pthread_mutex_destroy(&q->mutex);
  // pthread_cond_destroy(&q->cond);
  // sem_destroy(&q->sem);

  qnode_t *tmp = q->first;

  while (tmp) {
    qnode_t *next = tmp->next;

    free(tmp);
    tmp = next;
  }

  free(q);
}

int queue_add(queue_t *q, int val) {
  // pthread_spin_lock(&q->spinlock);
  pthread_mutex_lock(&q->mutex);
  // sem_wait(&q->sem);

  // while (q->count == q->max_count) {
  //   pthread_cond_wait(&q->cond, &q->mutex);
  // }

  struct timespec mt1, mt2; 
  clock_gettime(CLOCK_REALTIME, &mt1);


  q->add_attempts++;

  assert(q->count <= q->max_count);

  if (q->count == q->max_count) {
    clock_gettime(CLOCK_REALTIME, &mt2);
    q->add_avg_time += 1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);


    // pthread_spin_unlock(&q->spinlock);
    pthread_mutex_unlock(&q->mutex);
    // pthread_cond_signal(&q->cond);
    // sem_post(&q->sem);



    return 0;
  }

  qnode_t *new = malloc(sizeof(qnode_t));
  if (!new) {
    printf("Cannot allocate memory for new node\n");
    abort();
  }

  new->val = val;
  new->next = NULL;

  if (!q->first)
    q->first = q->last = new;
  else {
    q->last->next = new;
    q->last = q->last->next;
  }

  q->count++;
  q->add_count++;


  clock_gettime(CLOCK_REALTIME, &mt2);
  q->add_avg_time += 1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);


  // pthread_spin_unlock(&q->spinlock);
  pthread_mutex_unlock(&q->mutex);

  // pthread_cond_signal(&q->cond);

  // sem_post(&q->sem);
  return 1;
}

int queue_get(queue_t *q, int *val) {
  // pthread_spin_lock(&q->spinlock);
  pthread_mutex_lock(&q->mutex);
  // sem_wait(&q->sem);

  // while (q->count == 0) {
  //   pthread_cond_wait(&q->cond, &q->mutex);
  // }

  struct timespec mt1, mt2; 
  clock_gettime(CLOCK_REALTIME, &mt1);

  q->get_attempts++;

  assert(q->count >= 0);

  if (q->count == 0) {
    clock_gettime(CLOCK_REALTIME, &mt2);
    q->get_avg_time += 1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);

    // pthread_spin_unlock(&q->spinlock);
    pthread_mutex_unlock(&q->mutex);
    // pthread_cond_signal(&q->cond);

    // sem_post(&q->sem);


    return 0;
  }
  qnode_t *tmp = q->first;

  *val = tmp->val;
  q->first = q->first->next;

  free(tmp);
  q->count--;
  q->get_count++;

  clock_gettime(CLOCK_REALTIME, &mt2);
  q->get_avg_time +=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);

  // pthread_spin_unlock(&q->spinlock);
  pthread_mutex_unlock(&q->mutex);
  // pthread_cond_signal(&q->cond);
  // sem_post(&q->sem);

  return 1;
}

void queue_print_stats(queue_t *q) {
  // pthread_spin_lock(&q->spinlock);
  pthread_mutex_lock(&q->mutex);
  // sem_wait(&q->sem);
  printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld "
         "%ld %ld)\n",
         q->count, q->add_attempts, q->get_attempts,
         q->add_attempts - q->get_attempts, q->add_count, q->get_count,
         q->add_count - q->get_count);
  printf("add avg time: %lf\n", q->add_avg_time /  q->add_attempts);
  printf("get avg time: %lf\n", q->get_avg_time / q->get_attempts);

  // pthread_spin_unlock(&q->spinlock);
  pthread_mutex_unlock(&q->mutex);
  // sem_post(&q->sem)
}
