//
// Created by io on 04.03.2023.
//

#ifndef INC_2_MATRIX_OP_H
#define INC_2_MATRIX_OP_H

#include <vector>
#include <xmmintrin.h>
#include <iostream>

void matrix_vector_multiplication(std::vector<double> &A, std::vector<double> &vector, int line_size, int column_size,
                                  std::vector<double> &result);

void vector_scalar_multiplication(std::vector<double> &vector, double c, int N, std::vector<double> &result);

double inner2(double *x, double *y, int n);


void plusVec(double *x, double *y, double *res, int n);

void minusVec(double *x, double *y, double *res, int n);

#endif //INC_2_MATRIX_OP_H
