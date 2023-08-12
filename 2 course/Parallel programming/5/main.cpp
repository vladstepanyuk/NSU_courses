#include <iostream>
#include <vector>
#include <sstream>
#include <mpi.h>
#include "LifeConway.h"


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    if (argc < 3) {
        MPI_Finalize();
        return 0;
    }

    int xSize = atoi(argv[1]);
    int iterationsNum = atoi(argv[2]);

    std::stringstream istream;
    istream << "#S " << xSize <<  ' ' << xSize << "\n0 2\n1 0\n1 2\n2 1\n2 2";
    parallelLifeConway::LifeConway::exec(istream, iterationsNum);

    MPI_Finalize();
    return 0;
}
