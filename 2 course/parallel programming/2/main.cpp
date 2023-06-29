#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "matrix_op.h"
#include <ctime>


const int MAX_ITERATION = 50000;
const double epsilon = 0.000001;

std::vector<double> matrix_gen(int N) {
    std::vector<double> A(N * N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) A[i*N + j] = 2.0;
            else A[i*N + j] = 1.0;
        }
    }
    return A;
}

std::vector<double> vector_gen(std::vector<double> A, int N) {
    std::vector<double> u(N);
    std::vector<double> vector(N);
    for (int i = 0; i < N; ++i) {
        u[i] = sin(2*i*3.14/N);
    }
    for (int i = 0; i < N; ++i) {
        matrix_vector_multiplication(A, u, N, N, vector);
    }
    return vector;
}

void print_x(std::vector<double> x) {
    std::cout << "3 number from result vector:" << std::endl;
    std::cout << x[0] << ' ' << x[x.size()/2] << ' ' << x[x.size() - 1] << std::endl;
}


int main(int argc, char **argv) {
    if (argc != 4) return 0;
    int N = atoi(argv[1]);
    
    std::vector<double> A;
    std::vector<double> b;




    A = matrix_gen( N);
    b = vector_gen(A, N);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    double b_norm = inner2(&b[0], &b[0], N);

    std::vector<double> xn_1(N, 0);
    std::vector<double> yn_1(N, 0);
    std::vector<double> Ayn_1(N, 0);

    vector_scalar_multiplication(b, -1, N, yn_1);

    matrix_vector_multiplication(A, yn_1, N, N, Ayn_1);

    double yAy = inner2(&yn_1[0], &Ayn_1[0], N);
    double AyAy = inner2(&Ayn_1[0], &Ayn_1[0], N);

    double tn_1 = yAy / AyAy;

    std::vector<double> x_n(N);
    std::vector<double> tyn_1(N);

    vector_scalar_multiplication(yn_1, tn_1, N, tyn_1);
    minusVec(&xn_1[0], &tyn_1[0], &x_n[0], N);

    std::vector<double> Axn_b(N);
    std::vector<double> tAyn_1(N);
    vector_scalar_multiplication(Ayn_1, tn_1, N, tAyn_1);
    minusVec(&yn_1[0], &tAyn_1[0], &Axn_b[0], N);
    double Axn_b_norm = inner2(&Axn_b[0], &Axn_b[0], N);

    double border = epsilon * epsilon * b_norm * b_norm;

    for (int i = 0; (i < MAX_ITERATION)&& Axn_b_norm < border; ++i) {
        yn_1 = Axn_b;
        xn_1 = x_n;
        matrix_vector_multiplication(A, yn_1, N, N, Ayn_1);

        yAy = inner2(&yn_1[0], &Ayn_1[0], N);
        AyAy = inner2(&Ayn_1[0], &Ayn_1[0], N);

        tn_1 = yAy / AyAy;

        vector_scalar_multiplication(yn_1, tn_1, N, tyn_1);
        minusVec(& xn_1[0], &tyn_1[0], &x_n[0], N);

        vector_scalar_multiplication(Ayn_1, tn_1, N, tAyn_1);
        minusVec(&yn_1[0], &tAyn_1[0], &Axn_b[0], N);
        Axn_b_norm = inner2(&Axn_b[0], &Axn_b[0], N);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    std::cout << "Time taken: " << end.tv_sec-start.tv_sec + 0.000000001*(end.tv_nsec-start.tv_nsec) << std::endl;
    print_x(x_n);
	std::cout << "norm " << sqrt(Axn_b_norm) << std::endl;

    return 0;
}
