#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int global = 0;

struct str {
  int i;
  char *string;
};
typedef struct str str;

void exit_clean_up(void *arg) {
  printf("free memory\n");
  free(arg);
}

void signal_handler(int sig) { printf("handle %d signal\n", sig); }

void *mythread(void *arg) {
  int local = 0;
  static int static_local;
  const int const_local;
  if (gettid() % 2 == 0) {
    global = 2;
    local = 2;
  } else {
    sleep(1);
  }
  printf("[pid = %d, ppid = %d, pthread_id = %d, ttid = %d]: local = %p, "
         "static local = %p, const local = %p, global = %p, local value = %d, "
         "global value = %d\n",
         getpid(), getppid(), pthread_self(), gettid(), &local, &static_local,
         &const_local, &global, local, global);

  char *return_value = (char *)malloc(sizeof(char) * 13);

  char hello[] = "Hello world!";

  memcpy(return_value, hello, 13);

  // printf("\n1\n");
  pthread_exit((void *)return_value);
}

void *mythread2(void *arg) {
  // pthread_detach(pthread_self());
  printf("[pthread_id = %ld]\n", pthread_self());
  return NULL;
}

void *mythread3(void *arg) {
  str *sstruct = (str *)arg;
  sleep(1);

  printf("struct str {i = %d, string = \"%s\"}\n", sstruct->i, sstruct->string);
  return NULL;
}

void *mythread4(void *arg) {
  // int i = 0;
  // while (1)
  // {
  // 	pthread_testcancel();
  // 	i += 1;
  // }

  char *hello = (char *)malloc(13);

  memcpy(hello, "hello world!", 13);

  pthread_cleanup_push(exit_clean_up, hello);

  while (1) {
    printf("%s\n", hello);
  }

  free(hello);
  pthread_cleanup_pop(0);

  return NULL;
}

void *mythread5_1(void *arg) {

  sigset_t sigset;

  int err = sigfillset(&sigset);

  if (err) {
    printf("mythread5_1: sigset failed");
    pthread_exit(err);
  }

  pthread_sigmask(SIG_BLOCK, &sigset, NULL);

  sleep(5);

  printf("thread [%ld] done\n", pthread_self());
}

void *mythread5_2(void *arg) {
  struct sigaction act;

  act.sa_handler = signal_handler;
  act.sa_flags = SA_RESTART;
  sigfillset(&act.sa_mask);
  
  sigaction(SIGINT, &act, NULL);

  sleep(5);
}

void *mythread5_3(void *arg) {
  sigset_t sigset;

  int err = sigaddset(&sigset, SIGQUIT);

  if (err) {
    printf("mythread5_1: sigset failed\n");
    pthread_exit(err);
  }

  int sig;

  sigwait(&sigset, &sig);

  if (sig == SIGQUIT) {
    printf("thread [%ld] cautch SIGQUIT\n", pthread_self());
  }

  sleep(5);
}

void f1() {

  int k = 3;
  pthread_t tid[k];
  int err = 0;

  printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

  pthread_attr_t attribute;
  err = pthread_attr_init(&attribute);
  err += pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);
  for (int i = 0; i < k; i++) {
    err += pthread_create(&tid[i], &attribute, mythread, NULL);
  }
  // sleep(5);

  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  void *return_values[k];

  for (int i = 0; i < k; i++) {
    pthread_join(tid[i], &return_values[i]);
    printf("thread [%ld] return value: \"%s\" \n", tid[i], return_values[i]);
    free(return_values[i]);
  }
}

void f2() {
  int err = 0;
  pthread_attr_t attribute;
  err = pthread_attr_init(&attribute);
  err += pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
  while (1) {
    pthread_t tid;
    err = pthread_create(&tid, &attribute, mythread2, NULL);
    if (err) {
      printf("main: pthread_create() failed: %s\n", strerror(err));
      return;
    }
  }
}

void f3() {
  str sstruct;
  pthread_t tid;
  int err;

  char hello[] = "hello world!";

  sstruct.i = 100;
  sstruct.string = (char *)malloc(13);
  memcpy(sstruct.string, hello, 13);

  pthread_attr_t attribute;
  err = pthread_attr_init(&attribute);
  if (err) {
    printf("main: pthread_attr_init() failed: %s\n", strerror(err));
    free(sstruct.string);
    exit(err);
  }
  err = pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
  if (err) {
    printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
    pthread_attr_destroy(&attribute);
    free(sstruct.string);
    exit(err);
  }

  err = pthread_create(&tid, &attribute, mythread3, &sstruct);
  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    pthread_attr_destroy(&attribute);
    free(sstruct.string);
    exit(err);
  }

  // sleep(5);
  pthread_join(tid, NULL);

  pthread_attr_destroy(&attribute);
  free(sstruct.string);
}

void f4() {
  pthread_t tid;
  int err;
  err = pthread_create(&tid, NULL, mythread4, NULL);

  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    exit(err);
  }

  sleep(5);

  err = pthread_cancel(tid);

  if (err) {
    printf("main: pthread_cansel() failed: %s\n", strerror(err));
    exit(err);
  }

  pthread_join(tid, NULL);
}

void f5() {
  pthread_t tid1, tid2, tid3;

  int err = pthread_create(&tid1, NULL, mythread5_1, NULL);

  if (err) {
    printf("main: pthread_create() failed: %s\n", strerror(err));
    exit(err);
  }

  int err2 = pthread_create(&tid2, NULL, mythread5_2, NULL);

  if (err2) {
    printf("main: pthread_create() failed: %s\n", strerror(err2));
    exit(err2);
  }

  int err3 = pthread_create(&tid3, NULL, mythread5_3, NULL);

  if (err3) {
    printf("main: pthread_create() failed: %s\n", strerror(err3));
    exit(err3);
  }

  sleep(2);

  pthread_kill(tid1, SIGQUIT);
  pthread_kill(tid2, SIGINT);
  pthread_kill(tid3, SIGQUIT);
  // sleep(1);
  // pthread_kill(tid3, SIGINT);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);
}

int main() {

  f5();

  return 0;
}
