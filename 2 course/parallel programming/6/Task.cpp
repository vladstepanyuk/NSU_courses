//
// Created by io on 26.05.2023.
//

#include <cstdlib>
#include <cmath>
#include "Task.h"




Task::Task() {
    repeat_num = std::rand() %100000;
}


double Task::operator()() const {
    double result = 0;

    for (int i = 0; i < repeat_num; ++i) {
        result += sqrt(i);
    }

    return result;
}

std::vector<Task> Task::generateTaskList(int list_size) {
    return std::vector<Task>(list_size);
}

int Task::getRepeatNum() const {
    return repeat_num;
}
