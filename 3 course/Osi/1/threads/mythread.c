#define _GNU_SOURCE
#include <sched.h>

#include "mythread.h"
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

const int PAGE_SIZE = 4096;
const int STACK_SIZE = PAGE_SIZE * 8;

void *create_stack(off_t size, int thread_num) {
  int stack_fd;
  void *stack;
  char stack_file[100];
  snprintf(stack_file, 100, "./stack-%d", thread_num);

  stack_fd = open(stack_file, O_CREAT | O_RDWR, 0666);
  if (stack_fd == -1)
    return NULL;

  ftruncate(stack_fd, 0);
  ftruncate(stack_fd, size);

  stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, stack_fd, 0);
  if (mmap == MAP_FAILED)
    return NULL;

  close(stack_fd);
  memset(stack, 0, size);
  return stack;
}

int mythread_wrap_function(void *args) {
  mythread_struct_t *mythread = (mythread_struct_t *)args;

  void *result = mythread->func(mythread->arg);

  if (mythread->is_detached) {
    mythread->is_ended = true;
    free(mythread);
    return 0;
  }

  mythread->result = result;
  mythread->is_ended = true;
  while (!mythread->is_joined) {
    sleep(1);
  }

  return 0;
}

int mythread_create(mythread_t *tid, start_routine_t func, void *arg,
                    bool is_detached) {
  static int thread_num = 0;
  mythread_struct_t *mythread =
      (mythread_struct_t *)malloc(sizeof(mythread_struct_t));
  thread_num++;

  mythread->func = func;
  mythread->arg = arg;
  mythread->id = thread_num;
  mythread->is_detached = is_detached;
  mythread->is_ended = false;
  mythread->is_joined = false;
  void *stack = create_stack(STACK_SIZE, thread_num);

  if (stack == NULL) {
    return -1;
  }

  int child_pid =
      clone(mythread_wrap_function, stack + STACK_SIZE,
            CLONE_VM | CLONE_THREAD | CLONE_FILES | CLONE_FS | CLONE_SIGHAND,
            mythread);

  if (child_pid == -1) {
    return -1;
  }
  *tid = mythread;
  return 0;
}

int mythread_join(mythread_t tid, void **result) {
  mythread_struct_t *mythread = (mythread_struct_t *)tid;
  while (!mythread->is_ended) {
    sleep(1);
  }

  *result = mythread->result;

  mythread->is_joined = true;
  free(mythread);
  return 0;
}
