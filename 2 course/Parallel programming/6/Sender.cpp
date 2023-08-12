//
// Created by io on 26.05.2023.
//

#include "Sender.h"

void *sender(void *list) {
    auto tasks = (Tasks_list *) (list);
    int rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int rank_need_task;
    int task_size;
    std::vector<int> sizes(size, 0);
    for (int it = 0; it < tasks_lists_number; ++it) {
        while (true) {
            if (rank == 0) {
                MPI_Recv(&rank_need_task, 1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            MPI_Bcast(&rank_need_task, 1, MPI_INT, 0, MPI_COMM_WORLD);
            task_size = tasks->get_size();
            MPI_Allgather(&task_size, 1, MPI_INT, &sizes[0], 1, MPI_INT, MPI_COMM_WORLD);

            int argmax = -1;
            int max = -1;
            for (int i = 0; i < size; ++i) {
                if (sizes[i] > 3 && sizes[i] > max) {
                    max = sizes[i];
                    argmax = i;
                }
            }


            if (max == -1) {

                if (rank == 0) {
                    MPI_Send(&max, 1, MPI_INT, rank_need_task, 1, MPI_COMM_WORLD);
                }
                break;
            }

            if (argmax == rank) {

                auto third = tasks->pop_third();
                int third_size = third.size();
                MPI_Send(&third_size, 1, MPI_INT, rank_need_task, 1, MPI_COMM_WORLD);
                MPI_Send(&third[0], third.size() * sizeof(Task), MPI_BYTE, rank_need_task, 2, MPI_COMM_WORLD);
            }


        }

        for (int i = 1, max = -1; i < size - 1 && rank == 0; ++i) {
            MPI_Recv(&rank_need_task, 1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&max, 1, MPI_INT, rank_need_task, 1, MPI_COMM_WORLD);
        }
    }




    return nullptr;
}

