//
// Created by io on 28.04.2023.
//

#include "MPIManager.h"

namespace parallelLifeConway {
    void MPIManager::distributeFields(Field &fieldRoot, Field &fieldDestination) {
        int size, rank;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        std::vector<int> sendCounts(size, 0);
        std::vector<int> displs(size, 0);

        int ySizeSmall = fieldDestination.getYSize() - 2;
        int xSizeSmall = fieldDestination.getXSize();


        MPI_Gather(&ySizeSmall, 1, MPI_INT, &sendCounts[0], 1, MPI_INT, 0, MPI_COMM_WORLD);

        for (int i = 0; i < size; ++i)
            sendCounts[i] *= fieldDestination.getXSize();

        for (int i = 1; i < size; ++i)
            displs[i] = displs[i - 1] + sendCounts[i - 1];

        MPI_Scatterv(&fieldRoot.getField()[0], &sendCounts[0], &displs[0], MPI_CHAR,
                     &(fieldDestination.getField()[xSizeSmall]), ySizeSmall * fieldDestination.getXSize(),
                     MPI_CHAR, 0,
                     MPI_COMM_WORLD);
    }

    void MPIManager::BcastSize(int &xSize, int &ySize) {
        MPI_Bcast(&xSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&ySize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    void MPIManager::upDownSend(Field &fieldRoot, MPI_Request &send) {
        int size, rank;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        int upRank = (size + rank - 1) % size;
        int downRank = (size + rank + 1) % size;


        MPI_Isend(&fieldRoot.getField()[fieldRoot.getXSize()], fieldRoot.getXSize(), MPI_CHAR, upRank, 1, MPI_COMM_WORLD, &send);
        MPI_Isend(&fieldRoot.getField()[fieldRoot.getField().size() - 2 * fieldRoot.getXSize()], fieldRoot.getXSize(),
                  MPI_CHAR, downRank, 2, MPI_COMM_WORLD, &send);

    }

    void MPIManager::upDownReceive(Field &fieldRoot, MPI_Request &requestUp, MPI_Request &requestDown) {
        int size, rank;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        int upRank = (size + rank - 1) % size;
        int downRank = (size + rank + 1) % size;




        MPI_Irecv(&fieldRoot.getField()[0], fieldRoot.getXSize(), MPI_CHAR, upRank, 2, MPI_COMM_WORLD, &requestUp);
        MPI_Irecv(&fieldRoot.getField()[fieldRoot.getField().size() - fieldRoot.getXSize()], fieldRoot.getXSize(),
                  MPI_CHAR, downRank, 1, MPI_COMM_WORLD, &requestDown);
    }
} // parallelLifeConway