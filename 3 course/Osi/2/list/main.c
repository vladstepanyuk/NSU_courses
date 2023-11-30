#include <stdio.h>
#include <pthread.h>
#include "list.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef int (*cmp)(const char *, const char *);
int flag = 1;

int count_less = 0, count_eq = 0, count_great = 0;
int count_swap_1 = 0, count_swap_2 = 0, count_swap_3 = 0;

typedef struct _arg_1 {
  Storage *storage;
  cmp cmp_f;
  int *counter;
} arg_stat_checker;

typedef struct _arg_2 {
  Storage *storage;
  int *counter;
} arg_swaper;

void *stat_check_routine(void *args) {

  arg_stat_checker *args_st = (arg_stat_checker *) args;
  Storage *storage = args_st->storage;
  cmp cmp_f = args_st->cmp_f;

  Node *cur_node = storage->first;

  while (flag) {
    int count = 0;

    while (cur_node) {
      Lock *sync1 = &cur_node->sync;
//      printf("[thread %lu] trying to lock %p\n", pthread_self(), cur_node);
      lock(sync1, READ_LOCK);

      Node *next_node = cur_node->next;

      while (next_node) {
        Lock *sync2 = &next_node->sync;
//        printf("[thread %lu] trying to lock %p\n", pthread_self(), next_node);
        lock(sync2, READ_LOCK);

        if (cmp_f(cur_node->data, next_node->data))
          ++count;


//        printf("[thread %lu] unlock %p\n", pthread_self(), next_node);
        next_node = next_node->next;
        unlock(sync2);
      }

//      printf("[thread %lu] unlock %p\n", pthread_self(), cur_node);

      cur_node = cur_node->next;
      unlock(sync1);
      ++(*args_st->counter);

    }

//    printf("[thread %lu] %d\n", pthread_self(), count);
//    sleep(1);
    cur_node = storage->first;
  }

  return NULL;
}

double get_p() {
  return (double) rand() / RAND_MAX;
//  return 0 ;
}

void swap_nodes(Node *node1, Node *node2, Node *node3) {
  node1->next = node3;
  node2->next = node3->next;
  node3->next = node2;
}

void *swap_routine(void *args) {
  arg_swaper *args_sw = (arg_swaper *) args;
  Storage *storage = args_sw->storage;

  Node *cur_node = storage->first;
  while (flag) {
    while (cur_node) {
      get_p();
      Lock *sync1 = &cur_node->sync;
//      printf("[thread %lu] trying to lock %p\n", pthread_self(), cur_node);
      lock(sync1, WRITE_LOCK);
      if (get_p() < 0.7 && cur_node->next) {
        Node *next_node = cur_node->next;
//        printf("[thread %lu] trying to lock %p\n", pthread_self(), next_node);
        lock(&next_node->sync, WRITE_LOCK);

        if (next_node->next) {
          Node *nnext_node = next_node->next;
//          printf("[thread %lu] trying to lock %p\n", pthread_self(), nnext_node);
          lock(&nnext_node->sync, WRITE_LOCK);
          swap_nodes(cur_node, next_node, nnext_node);
          ++(*args_sw->counter);
          unlock(&nnext_node->sync);
//          printf("[thread %lu] unlock %p\n", pthread_self(), nnext_node);

        }

        unlock(&next_node->sync);
//        printf("[thread %lu] unlock %p\n", pthread_self(), next_node);

      }

      cur_node = cur_node->next;
      unlock(sync1);
//      printf("[thread %lu] unlock %p\n", pthread_self(), cur_node);

    }

    cur_node = storage->first;
  }

  return NULL;
}

int cmp_1(const char *str1, const char *str2) {
  return strlen(str1) < strlen(str2);
}

int cmp_2(const char *str1, const char *str2) {
  return strlen(str1) == strlen(str2);
}

int cmp_3(const char *str1, const char *str2) {
  return strlen(str1) > strlen(str2);
}

void *stat_printer(void *_){

  for (int i = 0; flag; ++i) {
    printf("[iteration %d]\n", i);
    printf("count_less %d, count_eq %d, count_great %d\n", count_less, count_eq, count_great);
    printf("swap_1 %d, swap_2 %d, swap_3 %d\n\n", count_swap_1, count_swap_2, count_swap_3);
    sleep(1);

  }

  return NULL;
}

int main() {
  Storage storage;
  initialize_storage(&storage);

  arg_stat_checker arg_1 = {&storage, cmp_1, &count_less};
  arg_stat_checker arg_2 = {&storage, cmp_2, &count_eq};
  arg_stat_checker arg_3 = {&storage, cmp_3, &count_great};

  arg_swaper arg_swaper_1 = {&storage, &count_swap_1};
  arg_swaper arg_swaper_2 = {&storage, &count_swap_2};
  arg_swaper arg_swaper_3 = {&storage, &count_swap_3};

  pthread_t tid_1, tid_2, tid_3;
  pthread_t stid_1, stid_2, stid_3;
  pthread_t stat_tid;

  pthread_create(&stat_tid, NULL, stat_printer, NULL);

  pthread_create(&stid_1, NULL, swap_routine, (void *) &arg_swaper_1);
  pthread_create(&stid_2, NULL, swap_routine, (void *) &arg_swaper_2);
  pthread_create(&stid_3, NULL, swap_routine, (void *) &arg_swaper_3);


  pthread_create(&tid_1, NULL, stat_check_routine, (void *) &arg_1);
  pthread_create(&tid_2, NULL, stat_check_routine, (void *) &arg_2);
  pthread_create(&tid_3, NULL, stat_check_routine, (void *) &arg_3);


  while (1) {
    char buf[100];
    scanf("%s", buf);
    if (strcmp(buf, "exit") == 0) {
      flag = 0;
      break;
    }
  }


  pthread_join(tid_1, NULL);
  pthread_join(tid_2, NULL);
  pthread_join(tid_3, NULL);
  pthread_join(stid_1, NULL);
  pthread_join(stid_2, NULL);
  pthread_join(stid_3, NULL);
  pthread_join(stat_tid, NULL);

  destroy_storage(&storage);
  return 0;
}


//[iteration 24]
//count_less 1366, count_eq 1291, count_great 1301
//swap_1 776508, swap_2 873071, swap_3 864130


//[iteration 24]
//count_less 639, count_eq 639, count_great 634
//swap_1 680803, swap_2 677677, swap_3 688935
