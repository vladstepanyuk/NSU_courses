#pragma once


#include <ucontext.h>
#include <unistd.h>


typedef void *(*start_routine_t)(void *);

struct uthread_struct_t{
    start_routine_t func;
    void *arg;
    void *result;
    void *stack;
    int id;
    bool is_ended;
//    bool is_joined;
    ucontext_t context;
    int i;
};


typedef uthread_struct_t *uthread_t;

void reschedule();

int uthread_create(uthread_t &tid, start_routine_t func, void *arg);

void uthread_join(uthread_t tid, void **result);