#ifndef INC_6_TASKS_LIST_H
#define INC_6_TASKS_LIST_H
#include "Task.h"
#include <pthread.h>

class Tasks_list {
    pthread_mutex_t mutex;

    std::vector<Task> tasks_list;

public:
    Tasks_list();

    explicit Tasks_list(int tasks_list_size);

    explicit Tasks_list(const std::vector<Task> &tasksList);

    int get_size();

    Task pop_task();

    void append(std::vector<Task> &tasks);

    bool is_empty();

    std::vector<Task> pop_third();

    Task &operator[](int i);

};


#endif //INC_6_TASKS_LIST_H
