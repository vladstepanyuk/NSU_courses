#pragma once

#include <stdbool.h>
typedef void *(*start_routine_t)(void *);

typedef struct mythread {
  start_routine_t func;
  void *arg;
  void *result;
  int id;
  bool is_ended;
  bool is_joined;
  bool is_detached;
} mythread_struct_t;

typedef mythread_struct_t *mythread_t;

int mythread_create(mythread_t *tid, start_routine_t func, void *arg, bool is_detached);

int mythread_join(mythread_t tid, void **result);