#ifndef LIST__LIST_H_
#define LIST__LIST_H_

#include <pthread.h>


enum LOCK_MODE {
    READ_LOCK,
    WRITE_LOCK
};

typedef struct _Lock {
//    pthread_spinlock_t sync;
//    pthread_mutex_t sync;

    pthread_rwlock_t sync;
} Lock;

void initialize_lock(Lock *lock);

void destroy_lock(Lock *lock);

void lock(Lock *lock, int flag);

void unlock(Lock *lock);

typedef struct _Node {
  char data[100];
  struct _Node *next;
  Lock sync;
} Node;

typedef struct _Storage {
  Node *first;
} Storage;

int initialize_storage(Storage *storage);

int push(Node *node,const char *value);

void destroy_storage(Storage *storage);

#endif //LIST__LIST_H_
