#include "matrix_op.h"

void matrix_vector_multiplication(std::vector<double> &A, std::vector<double> &vector, int line_size, int column_size,
                                  std::vector<double> &result) {
    for (int i = 0; i < column_size; ++i) {
        result[i] = inner2(&A[line_size * i], &vector[0], line_size);
    }
}

void vector_scalar_multiplication(std::vector<double> &vector, double c, int N, std::vector<double> &result) {

    for (int i = 0; i < N; ++i)
        result[i] = c * vector[i];

}

double inner2(double *x, double *y, int n) {
    double sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += x[i] * y[i];
    }

    return sum;
}

void plusVec(double *x, double *y, double *res, int n) {
    for (int i = 0; i < n; ++i) {
        res[i] = x[i] + y[i];
    }
}
void minusVec(double *x, double *y, double *res, int n) {

    for (int i = 0; i < n; ++i) {
        res[i] = x[i] - y[i];
    }
}