#ifndef INC_6_TASK_H
#define INC_6_TASK_H

#include <vector>

class Task {
private:
    int repeat_num;
public:
    Task();

    static std::vector<Task> generateTaskList(int list_size);

    int getRepeatNum() const;

    double operator() () const;
};


#endif //INC_6_TASK_H
