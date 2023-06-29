#ifndef INC_6_SENDER_H
#define INC_6_SENDER_H
#include <iostream>
#include <cmath>
#include "mpi.h"
#include "Tasks_list.h"
#include "Mpi_manager.h"


const int tasks_lists_number = 60;


void *sender(void *list);

#endif //INC_6_SENDER_H
