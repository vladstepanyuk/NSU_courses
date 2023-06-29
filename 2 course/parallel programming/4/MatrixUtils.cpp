//
// Created by io on 14.04.2023.
//

#include "MatrixUtils.h"

std::vector<double> MatrixUtils::matrixGen(int xSize, int ySize) {
    std::vector<double> matrix(xSize * ySize);
    srand(1500);
    for (int i = 0; i < xSize * ySize; ++i) {
        matrix[i] = rand() % 15;
    }
    return matrix;
}

void MatrixUtils::printMatrix(std::vector<double> &matrix, int xSize, int ySize) {
    for (int j = 0; j < ySize; ++j) {
        for (int k = 0; k < xSize; ++k) {
            std::cout << matrix[j * xSize + k] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;

}

void MatrixUtils::distributeMatrices(std::vector<double> &A, std::vector<double> &B, std::vector<double> &A_part,
                                     std::vector<double> &B_part, MPI_Comm &columnComm, MPI_Comm &lineComm,
                                     int n1_base, int n2, int n3_base, int n3, int rankx, int ranky) {
    MPI_Datatype col, coltype, coltype1;
    MPI_Type_vector(n2, 1, n3, MPI_DOUBLE, &col);
    MPI_Type_create_resized(col, 0, 1 * sizeof(double), &coltype);
    MPI_Type_commit(&coltype);
    MPI_Type_free(&col);

    MPI_Type_vector(n2, 1, n3_base, MPI_DOUBLE, &col);
    MPI_Type_create_resized(col, 0, 1 * sizeof(double), &coltype1);
    MPI_Type_commit(&coltype1);
    MPI_Type_free(&col);

    if (rankx == 0) {
        MPI_Scatter(&A[0], n1_base * n2, MPI_DOUBLE, &A_part[0], n1_base * n2, MPI_DOUBLE, 0, columnComm);
    }
    if (ranky == 0) {
        MPI_Scatter(&B[0], n3_base, coltype, &B_part[0], n3_base, coltype1, 0, lineComm);
    }

    MPI_Bcast(&A_part[0], A_part.size(), MPI_DOUBLE, 0, lineComm);
    MPI_Bcast(&B_part[0], B_part.size(), MPI_DOUBLE, 0, columnComm);
    MPI_Type_free(&coltype);
    MPI_Type_free(&coltype1);
}

void MatrixUtils::matricesMultiplication(std::vector<double> &A, std::vector<double> &B, std::vector<double> &C, int n1,
                                         int n2, int n3) {
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n3; ++j) {
            for (int k = 0; k < n2; ++k) {
                C[i * n3 + j] += A[i * n2 + k] * B[k * n3 + j];
            }
        }
    }
}

void
MatrixUtils::gatherMatrix(std::vector<double> &C, std::vector<double> &C_part, MPI_Comm &columnComm, MPI_Comm &lineComm,
                          int n1_base, int n3_base, int n3, int rankx, int ranky, int sizex, int sizey) {
    MPI_Datatype tmp, matrixLinesSend, matrixLinesReceive;
    MPI_Type_vector(n1_base, n3_base, n3_base, MPI_DOUBLE, &tmp);
    MPI_Type_create_resized(tmp, 0, 1 * sizeof(double), &matrixLinesSend);
    MPI_Type_commit(&matrixLinesSend);
    MPI_Type_free(&tmp);
    MPI_Type_vector(n1_base, n3_base, n3, MPI_DOUBLE, &tmp);
    MPI_Type_create_resized(tmp, 0, n3_base * sizeof(double), &matrixLinesReceive);
    MPI_Type_commit(&matrixLinesReceive);
    MPI_Type_free(&tmp);


    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> displs(size);
    std::vector<int> count(size, 1);

    for (int i = 0; i < size; i++) {
        displs[i] = 1 * (i % sizex) + (i / sizex) * n1_base * sizex;
    }
    std::cout << std::endl;


    MPI_Gatherv(&C_part[0], 1, matrixLinesSend, &C[0], &count[0], &displs[0], matrixLinesReceive, 0, MPI_COMM_WORLD);


    MPI_Type_free(&matrixLinesSend);
    MPI_Type_free(&matrixLinesReceive);
}

