#include "mythread.h"
#include <stdio.h>
#include <unistd.h>

void *child_func1(void *args) {
  int some_num = (int)args;
  printf("hello world from thread, number %d\n", some_num);
  return (void *)1234;
}
int main(int argc, char const *argv[]) {
  mythread_t tid;
  if (mythread_create(&tid, child_func1, (void *)123, false)) {
    printf("failed to create thread\n");
    return 1;
  }
  void *result;
  mythread_join(tid, &result);
  printf("%d\n", result);
  return 0;
}
