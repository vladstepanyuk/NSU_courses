#define _GNU_SOURCE
#include "mythread.h"
#include <stdio.h>
#include <unistd.h>

void *child_func1(void *args) {
  int some_num = (int)args;
  printf("[ppid: %d pid: %d tid: %d] hello world from thread, number %d\n",getppid(), getpid(), gettid(), some_num);
  return args;
}
int main(int argc, char const *argv[]) {
  mythread_t tid, tid1;

//  mythread_create(&tid, child_func1, (void *)123, true);

  while (true) {
    mythread_create(&tid, child_func1, (void *)123, false);
  }
//  if (mythread_create(&tid, child_func1, (void *)123, false)) {
//    printf("failed to create thread\n");
//    return 1;
//  }
//  if (mythread_create(&tid1, child_func1, (void *)1214634, false)) {
//    printf("failed to create thread\n");
//    return 1;
//  }
//  void *result;
//  mythread_join(tid, &result);
//  printf("%d\n", (int)result);
//  //  sleep(2);
//  mythread_join(tid1, &result);
//  printf("%d\n", result);
  return 0;
}
