
#include <algorithm>
#include <cmath>
#include "LifeConway.h"

namespace parallelLifeConway {
    void LifeConway::exec(std::istream &istream, int MaxIteration) {
        int size, rank;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        Field fieldBig, fieldSmall;
        int xSizeBig, ySizeBig;
        int xSizeSmall, ySizeSmall;
        double rowPerProc;


        if (rank == 0) {
            fieldBig = InputFileParser::pars(istream);
            xSizeBig = fieldBig.getXSize();
            ySizeBig = fieldBig.getYSize();
        }

        MPIManager::BcastSize(xSizeBig, ySizeBig);

        rowPerProc = (ySizeBig * 1.0) / size;

        xSizeSmall = xSizeBig;
        ySizeSmall = std::ceil(rowPerProc * (rank + 1)) - std::ceil(rowPerProc * (rank));

        fieldSmall = Field(std::vector<char>(xSizeSmall * (ySizeSmall + 2), 0), xSizeSmall, ySizeSmall + 2);

        MPIManager::distributeFields(fieldBig, fieldSmall);


        std::vector<Field> history(MaxIteration + 1);

        std::vector<char> iterationsEquiv(MaxIteration);
        std::pair<bool, int> pair = {false, 0};


        auto start = MPI_Wtime();

        std::vector<char> flags(MaxIteration);
        MPI_Request request = 0;


        int i;
        for (i = 0; i < MaxIteration; ++i) {
            history[i] = fieldSmall;
            MPI_Request send, receiveUp, receiveDown;

            MPIManager::upDownSend(fieldSmall, send);
            MPIManager::upDownReceive(history[i], receiveUp, receiveDown);


            fieldSmall.tickInternal(history[i]);

            MPI_Wait(&receiveUp, MPI_STATUS_IGNORE);
            MPI_Wait(&receiveDown, MPI_STATUS_IGNORE);

            fieldSmall.tickExternal(history[i]);


            if (request != 0) {
                MPI_Wait(&request, MPI_STATUS_IGNORE);
                pair = checkCondition(i, flags);
            }

            reduceFlags(history, fieldSmall, i + 1, flags, request);

            if (pair.first) break;
        }

        if (i == MaxIteration && request != 0) {
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            pair = checkCondition(MaxIteration, flags);
        }


        if (rank == 0) {
            auto end = MPI_Wtime();
            if (pair.first) {
                std::cout << "iteration reply: " << i << ":" << pair.second << std::endl << "time: " << end - start
                          << std::endl;
            } else {
                std::cout << "no reply" << std::endl << "time: " << end - start << std::endl;
            }
        }


    }

    void
    LifeConway::reduceFlags(std::vector<Field> &history, Field &field, int size, std::vector<char> &flags,
                            MPI_Request &request) {


        for (int i = 0; i < size; ++i) {
            flags[i] = history[i] == field;
        }

        MPI_Iallreduce(MPI_IN_PLACE, &flags[0], size, MPI_CXX_BOOL, MPI_LAND, MPI_COMM_WORLD, &request);

    }

    std::pair<bool, int> LifeConway::checkCondition(int size, std::vector<char> &flags) {
        for (int i = 0; i < size; ++i) {
            if (flags[i]) {
                return {true, i+1};
            }
        }

        return {false, 0};
    }
} // LifeConway