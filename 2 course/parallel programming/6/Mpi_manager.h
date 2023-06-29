//
// Created by io on 26.05.2023.
//

#ifndef INC_6_MPI_MANAGER_H
#define INC_6_MPI_MANAGER_H


#include "Tasks_list.h"

class Mpi_manager {
public:
    static void broadcast_size(int &size);

    static void scatter_task_list(Tasks_list &tasks_list_source, Tasks_list &tasks_list_destination, int task_size_dest);

};


#endif //INC_6_MPI_MANAGER_H
