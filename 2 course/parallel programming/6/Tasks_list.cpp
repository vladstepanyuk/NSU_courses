//
// Created by io on 26.05.2023.
//

#include <algorithm>
#include "Tasks_list.h"


Task Tasks_list::pop_task() {
    pthread_mutex_lock(&mutex);

    Task task = tasks_list.back();
    tasks_list.pop_back();

    pthread_mutex_unlock(&mutex);

    return task;
}

Tasks_list::Tasks_list(const std::vector<Task> &tasksList) : tasks_list(tasksList) {
    pthread_mutex_init(&mutex, nullptr);
}

Tasks_list::Tasks_list(int tasks_list_size) {
    pthread_mutex_init(&mutex, nullptr);
    tasks_list = std::vector<Task>(tasks_list_size);
    std::sort(tasks_list.begin(), tasks_list.end(), [](Task x, Task y) {return x.getRepeatNum() > y.getRepeatNum();});
}

int Tasks_list::get_size() {
    return tasks_list.size();
}

Tasks_list::Tasks_list() {
    pthread_mutex_init(&mutex, nullptr);
}

Task &Tasks_list::operator[](int i) {
    return tasks_list[i];
}

std::vector<Task> Tasks_list::pop_third() {
    pthread_mutex_lock(&mutex);

    int third = tasks_list.size() / 3;


    auto task = std::vector<Task>(third);
    for (int i = 0, j = tasks_list.size() - 1; i < third; ++i, --j) {
        task[i] = tasks_list[j];
    }

    tasks_list.resize(tasks_list.size() - third);

    pthread_mutex_unlock(&mutex);

    return task;
}

void Tasks_list::append(std::vector<Task> &tasks) {
//    pthread_mutex_lock(&mutex);

    for (auto & task : tasks) {
        tasks_list.emplace_back(task);
    }

//    pthread_mutex_unlock(&mutex);

}

bool Tasks_list::is_empty() {
    return tasks_list.empty();
}
