#include <algorithm>
#include "Sender.h"


std::string format_info_string(int i, double result, double time, int rank, int done) {
    std::string info;
    info += "iteration: " + std::to_string(i) + "\n";
    info += "rank: " + std::to_string(rank) + "\n";
    info += "time: " + std::to_string(time) + "\n";
    info += "result: " + std::to_string(result) + "\n";
    info += "done: " + std::to_string(done) + "\n";


    return info;
}


int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    try {
        if (argc < 2) {
            return 0;
        }


        int rank, size;


        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);


        int all_tasks_list_size = atoi(argv[1]);;
        int task_list_size;
        std::vector<Tasks_list> all_tasks_lists(tasks_lists_number);
        Tasks_list all_tasks_list;

        if (rank == 0) {
            std::srand(1024);

            for (int i = 0; i < tasks_lists_number; ++i) {
                all_tasks_lists[i] = Tasks_list(all_tasks_list_size);
            }
        }


        double rowPerProc = (all_tasks_list_size * 1.0) / size;
        task_list_size = std::ceil(rowPerProc * (rank + 1)) - std::ceil(rowPerProc * (rank));


        std::vector<Tasks_list> tasks_lists(tasks_lists_number);
        Tasks_list curr_tasks_list;


        for (int i = 0; i < tasks_lists_number; ++i) {
            Mpi_manager::scatter_task_list(all_tasks_lists[i], tasks_lists[i], task_list_size);
        }


        pthread_attr_t attrs;

        pthread_attr_init(&attrs);
        pthread_t thread;
        pthread_create(&thread, &attrs, sender, &curr_tasks_list);


        double global_res = 0;


        for (int i = 0; i < tasks_lists_number; i++) {
//            std::cout << i;
            curr_tasks_list = tasks_lists[i];
            double start = MPI_Wtime();
            double result = 0;
            int done = 0;


            while (true) {
                for (; !curr_tasks_list.is_empty();) {
                    result += curr_tasks_list.pop_task()();
                    done++;
                }


                if (rank != 0) {
                    MPI_Send(&rank, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
                } else {
                    break;
                }


                int number_of_el;

                MPI_Recv(&number_of_el, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


                if (number_of_el == -1) {
                    break;
                }

                std::vector<Task> tasks(number_of_el);

                MPI_Recv(&tasks[0], number_of_el * sizeof(Task), MPI_BYTE, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                curr_tasks_list.append(tasks);
            }

            double time = MPI_Wtime() - start;

            MPI_Allreduce(MPI_IN_PLACE, &result, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

            double min_time, max_time;

            MPI_Reduce(&time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
            MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


            global_res += result;
            for (int j = 0; j < size; ++j) {
                if (rank == j) {
                    std::cout << format_info_string(i, global_res, time, rank, done) << std::endl;
                }
            }

            if (rank == 0) {

                std::cout << "dis balance " +std::to_string(max_time - min_time)  + "\n" ;
                std::cout << "dis balance part "  +std::to_string((max_time - min_time) / max_time * 100)  + "\n";
            }

        }


        pthread_join(thread, nullptr);

        MPI_Finalize();
    } catch (std::exception &e) {
        std::cerr << e.what();
    }

    return 0;
}
