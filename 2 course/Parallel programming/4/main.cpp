#include "mpi.h"
#include "Solution.h"

namespace {
    const int MIN_ARGC = 3;
    const int n1 = 2400 * 4;
    const int n2 = 3;
    const int n3 = 2400 * 6;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<double> A, B;
    if (rank == 0) {
        A = MatrixUtils::matrixGen(n2, n1);
//        MatrixUtils::printMatrix(A, n2, n1);

        B = MatrixUtils::matrixGen(n3, n2);
//        MatrixUtils::printMatrix(B, n3, n2);

    }

    int p1[] = {2, 3, 4, 6, 8, 12};
    for (int i = 0; i < 6; i++) {
        Solution::matrix_mult(p1[i], 24 / p1[i], A, B, n1, n2, n3);
    }
//    Solution::matrix_mult(2, 3, A, B, n1, n2, n3);


    MPI_Finalize();
    return 0;
}
