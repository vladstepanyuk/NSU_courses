//
// Created by io on 14.04.2023.
//

#ifndef INC_4_MATRIXUTILS_H
#define INC_4_MATRIXUTILS_H

#include <vector>
#include <cstdlib>
#include <iostream>
#include <mpi.h>


class MatrixUtils {
public:
    static std::vector<double> matrixGen(int xSize, int ySize);

    static void printMatrix(std::vector<double> &matrix, int xSize, int ySize);

    static void distributeMatrices(std::vector<double> &A, std::vector<double> &B, std::vector<double> &A_part,
                                   std::vector<double> &B_part, MPI_Comm &columnComm, MPI_Comm &lineComm, int n1_base,
                                   int n2, int n3_base, int n3, int rankx, int ranky);

    static void matricesMultiplication(std::vector<double> &A, std::vector<double> &B, std::vector<double> &C, int n1,
                                       int n2, int n3);
    static void
    gatherMatrix(std::vector<double> &C, std::vector<double> &C_part, MPI_Comm &columnComm, MPI_Comm &lineComm,
                 int n1_base, int n3_base, int n3, int rankx, int ranky, int sizex, int sizey);
};


#endif //INC_4_MATRIXUTILS_H
