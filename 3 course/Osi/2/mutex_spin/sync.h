#pragma once

typedef struct _spinlock {
  int locked;
} spinlock_t;

int spinlock_init(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

typedef struct _mutex {
  int locked;
} mutex_t;


int mutex_init(mutex_t *lock);
void mutex_lock(mutex_t *lock);
void mutex_unlock(mutex_t *lock);