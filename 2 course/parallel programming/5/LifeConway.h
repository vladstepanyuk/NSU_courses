

#ifndef INC_5_LIFECONWAY_H
#define INC_5_LIFECONWAY_H


#include <iostream>
#include "Field.h"
#include "InputFileParser.h"
#include <list>
#include <mpi.h>
#include "MPIManager.h"


namespace parallelLifeConway {

    class LifeConway {
    public:
        static void exec(std::istream &istream, int MaxIteration);
    private:
        static void
        reduceFlags(std::vector<Field> &history, Field &field, int size, std::vector<char> &flags,
                    MPI_Request &request);
        static std::pair<bool, int> checkCondition(int size, std::vector<char> &flags);
    };

} // LifeConway


#endif
