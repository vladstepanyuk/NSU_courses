//
// Created by io on 26.05.2023.
//

#include <mpi.h>
#include "Mpi_manager.h"



void Mpi_manager::broadcast_size(int &size) {
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

}

void Mpi_manager::scatter_task_list(Tasks_list &tasks_list_source, Tasks_list &tasks_list_destination, int task_size_dest) {
    int rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<Task> dest_vector(task_size_dest);

    task_size_dest *= sizeof(Task);


    std::vector<int> sendCounts(size, 0);
    std::vector<int> displs(size, 0);


    MPI_Gather(&task_size_dest, 1, MPI_INT, &sendCounts[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 1; i < size; ++i)
        displs[i] = displs[i - 1] + sendCounts[i - 1];


    MPI_Scatterv(&tasks_list_source[0], &sendCounts[0], &displs[0], MPI_BYTE, &dest_vector[0],
                 task_size_dest, MPI_BYTE, 0, MPI_COMM_WORLD);

    tasks_list_destination = std::move(Tasks_list(dest_vector));
}
