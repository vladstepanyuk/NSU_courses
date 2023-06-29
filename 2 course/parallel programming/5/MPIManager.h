#ifndef INC_5_MPIMANAGER_H
#define INC_5_MPIMANAGER_H
#include "Field.h"
#include "mpi.h"

namespace parallelLifeConway {

    class MPIManager {
    public:
        static void BcastSize(int &xSize, int &ySize);
        static void distributeFields(Field &fieldRoot, Field &fieldDestination);
        static void upDownSend(Field &fieldRoot, MPI_Request& send);
        static void upDownReceive(Field &fieldRoot, MPI_Request& requestUp, MPI_Request& requestDown);

    };

} // parallelLifeConway

#endif //INC_5_MPIMANAGER_H
