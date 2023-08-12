//
// Created by io on 13.04.2023.
//

#include "Solution.h"


void
Solution::matrix_mult(int sizey, int sizex, std::vector<double> &A, std::vector<double> &B, int n1, int n2, int n3) {

    int dims[2] = {sizey, sizex}, periods[2] = {0, 0}, coords[2], reorder = 0;
    int ranky, rankx, rank;
    MPI_Comm comm2d;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm2d);
    MPI_Comm_rank(comm2d, &rank);
    MPI_Cart_get(comm2d, 2, dims, periods, coords);
    ranky = coords[0];
    rankx = coords[1];

    MPI_Comm columnComm;
    MPI_Comm lineComm;

    MPI_Comm_split(comm2d, rankx, ranky, &columnComm);
    MPI_Comm_split(comm2d, ranky, rankx, &lineComm);
    int n1_base = n1 / sizey, n2_base = n2, n3_base = n3 / sizex;


    std::vector<double> A_part(n1_base * n2_base, 0);
    std::vector<double> B_part(n3_base * n2_base, 0);
    double start = MPI_Wtime();
//    std::cout << 2;

    MatrixUtils::distributeMatrices(A, B, A_part, B_part, columnComm, lineComm, n1_base, n2, n3_base, n3, rankx, ranky);

    std::vector<double> C_part(n1_base * n3_base, 0);
    MatrixUtils::matricesMultiplication(A_part, B_part, C_part, n1_base, n2_base, n3_base);

    std::vector<double> C;
    if (rank == 0) {
        C = std::vector<double>(n1 * n3, 0);
    }
//    std::cout << 1;
    MatrixUtils::gatherMatrix(C, C_part, columnComm, lineComm, n1_base, n3_base, n3, rankx, ranky, sizex, sizey);
    if (rankx == 0 && ranky == 0) {
//        MatrixUtils::printMatrix(C, n3, n1);
        std::cout << sizex << "x" << sizey << std::endl;
        std::cout << "time: " << MPI_Wtime() - start << std::endl;
        std::cout << "2 numbers from matrix:\n" << C[0] << ' ' << C[n1 * n2 / 2] << std::endl << std::endl;
    }
//    std::cout << 1;


    MPI_Comm_free(&comm2d);
    MPI_Comm_free(&lineComm);
    MPI_Comm_free(&columnComm);


}

