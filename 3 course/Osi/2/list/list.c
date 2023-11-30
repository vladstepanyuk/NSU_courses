//
// Created by io on 28.11.2023.
//

#include "list.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define data_size 100000

int initialize_storage(Storage *storage) {
//  static char initialize_data[data_size][100] = {"a", "ab", "abc", "abcd", "abcde"};
  static char initialize_data[data_size][100];


  for (int i = 0; i < data_size; ++i) {
    initialize_data[i][99] = '\0';
  }

  storage->first = (Node *) malloc(sizeof(Node));

  if (!storage->first)
    return -1;

  strcpy(storage->first->data, initialize_data[0]);
  initialize_lock(&storage->first->sync);

  Node *curr = storage->first;
  for (int i = 1; i < data_size; ++i) {
    int err = push(curr, initialize_data[i]);

    if (err) {
      destroy_storage(storage);
      return -1;
    }

    curr = curr->next;
  }
  return 0;
}

int push(Node *node, const char *value) {
  lock(&node->sync, WRITE_LOCK);

  Node *new_node = (Node *) malloc(sizeof(Node));
  if (!new_node) {
    unlock(&node->sync);
    return -1;
  }

  initialize_lock(&new_node->sync);
  strcpy(new_node->data, value);

  Node *next_node = node->next;

  if (next_node) {
    lock(&next_node->sync, WRITE_LOCK);
  }

  new_node->next = next_node;

  node->next = new_node;

  if (next_node) {
    unlock(&next_node->sync);
  }

  unlock(&node->sync);
  return 0;
}

void destroy_storage(Storage *storage) {
  Node *curr_node = storage->first;

  while (curr_node) {
    Node *next_node = curr_node->next;
    destroy_lock(&curr_node->sync);
    free(curr_node);
    curr_node = next_node;
  }
}

void initialize_lock(Lock *lock) {
//  pthread_mutex_init(&lock->sync, NULL);
//  pthread_spin_init(&lock->sync, 0);

  pthread_rwlock_init(&lock->sync, NULL);
}
void destroy_lock(Lock *lock) {
//  pthread_mutex_destroy(&lock->sync);
//  pthread_spin_destroy(&lock->sync);

  pthread_rwlock_destroy(&lock->sync);
}

void lock(Lock *lock, int flag) {
//  pthread_mutex_lock(&lock->sync);
//
//  pthread_spin_lock(&lock->sync);

  switch (flag) {
    case READ_LOCK:
      pthread_rwlock_rdlock(&lock->sync);
      break;
    case WRITE_LOCK:
      pthread_rwlock_wrlock(&lock->sync);
      break;
    default:
      break;
  }
}

void unlock(Lock *lock) {
//  pthread_mutex_unlock(&lock->sync);
  pthread_rwlock_unlock(&lock->sync);
//  pthread_spin_unlock(&lock->sync);
}


