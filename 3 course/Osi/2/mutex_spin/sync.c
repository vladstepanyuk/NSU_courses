//
// Created by 1 on 30.11.2023.
//

#include <stdatomic.h>
#include <unistd.h>
#include <linux/futex.h>
#include "sync.h"
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <string.h>
int spinlock_init(spinlock_t *lock) {
  lock->locked = 0;
  return 0;
}

void spinlock_lock(spinlock_t *lock) {
  while (1) {
	int zero = 0;

	if (atomic_compare_exchange_strong(&lock->locked, &zero, 1)) {
	  break;
	}
  }

  lock->locked = 1;
}

void spinlock_unlock(spinlock_t *lock) {
  int one = 1;
  atomic_compare_exchange_strong(&lock->locked, &one, 0);
}

static long futex(int *addr, int op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
  return syscall(SYS_futex, addr, op, val, timeout, uaddr2, val3);
}

int mutex_init(mutex_t *lock) {
  lock->locked = 1;
  return 0;
}

void mutex_lock(mutex_t *lock) {
  while (1) {
	int one = 1;

	if (atomic_compare_exchange_strong(&lock->locked, &one, 0)) {
	  break;
	}

	long err = futex(&lock->locked, FUTEX_WAIT_PRIVATE, 0, NULL, NULL, 0);
	if (err == -1 && errno != EAGAIN) {
	  printf("futex wait failed. errno %s\n", strerror( errno ));
	  exit(1222);
	}
  }
}
void mutex_unlock(mutex_t *lock) {

  int zero = 0;
  if (atomic_compare_exchange_strong(&lock->locked, &zero, 1)) {

	long err = futex(&lock->locked, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
	if (err == -1) {
	  printf("futex wake failed\n");
	  exit(1);
	}
  }
}



