#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "matrix_op.h"
#include <cstdlib>

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
    MPI_Init(&argc, &argv);
    double start;
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (argc != 4) return 0;
    int N = atoi(argv[1]);
    
    std::vector<int> displsA(size);
    std::vector<int> send_countsA(size, (N / size) * N);
    for (int i = 0; i < size; ++i)
        displsA[i] = ((N / size)) * i * N;
    send_countsA[size - 1] = (N / size + N % size) * N;

    std::vector<int> displsB(size);
    std::vector<int> send_countsB(size, (N / size));
    for (int i = 0; i < size; ++i)
        displsB[i] = ((N / size)) * i;
    send_countsB[size - 1] = (N / size + N % size);

    std::vector<double> A;
    std::vector<double> b;




    if (rank == 0) {

        A = matrix_gen( N);

        b = vector_gen(A, N);
        MPI_Scatterv(&A[0], &send_countsA[0], &displsA[0], MPI_DOUBLE, MPI_IN_PLACE, (N / size) * N, MPI_DOUBLE, 0,
                     MPI_COMM_WORLD);
        MPI_Scatterv(&b[0], &send_countsB[0], &displsB[0], MPI_DOUBLE, MPI_IN_PLACE, (N / size), MPI_DOUBLE, 0,
                     MPI_COMM_WORLD);
    } else {
        A = std::vector<double>(send_countsA[rank]);
        b = std::vector<double>(send_countsB[rank]);
        MPI_Scatterv(&A[0], &send_countsA[0], &displsA[0], MPI_DOUBLE, &A[0], send_countsA[rank], MPI_DOUBLE, 0,
                     MPI_COMM_WORLD);
        MPI_Scatterv(&b[0], &send_countsB[0], &displsB[0], MPI_DOUBLE, &b[0], send_countsB[rank], MPI_DOUBLE, 0,
                     MPI_COMM_WORLD);
    }
    start = MPI_Wtime();
    double b_norm_p = inner2(&b[0], &b[0], send_countsB[rank]);
    double b_norm;
    MPI_Allreduce(&b_norm_p, &b_norm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    std::vector<double> xn_1(N, 0);
    std::vector<double> yn_1_p(send_countsB[rank], 0);
    std::vector<double> yn_1(N, 0);
    std::vector<double> Ayn_1(send_countsB[rank], 0);

    vector_scalar_multiplication(b, -1, send_countsB[rank], yn_1_p);
    MPI_Allgatherv(&yn_1_p[0], send_countsB[rank], MPI_DOUBLE, &yn_1[0], &send_countsB[0], &displsB[0], MPI_DOUBLE,
                   MPI_COMM_WORLD);

    matrix_vector_multiplication(A, yn_1, N, send_countsB[rank], Ayn_1);

    double yAy_p = inner2(&yn_1_p[0], &Ayn_1[0], send_countsB[rank]);
    double AyAy_p = inner2(&Ayn_1[0], &Ayn_1[0], send_countsB[rank]);
    double yAy, AyAy;


    MPI_Allreduce(&yAy_p, &yAy, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&AyAy_p, &AyAy, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);


    double tn_1 = yAy / AyAy;

    std::vector<double> x_n(N);
    std::vector<double> x_n_p(N);
    std::vector<double> tyn_1_p(send_countsB[rank]);



    vector_scalar_multiplication(yn_1_p, tn_1, send_countsB[rank], tyn_1_p);
    minusVec(&xn_1[displsB[rank]], &tyn_1_p[0], &x_n_p[0], send_countsB[rank]);
    MPI_Allgatherv(&x_n_p[0], send_countsB[rank], MPI_DOUBLE, &x_n[0], &send_countsB[0], &displsB[0], MPI_DOUBLE,
                   MPI_COMM_WORLD);




    std::vector<double> Axn_b(send_countsB[rank]);
    std::vector<double> tAyn_1(send_countsB[rank]);
    vector_scalar_multiplication(Ayn_1, tn_1, send_countsB[rank], tAyn_1);
    minusVec(&yn_1_p[0], &tAyn_1[0], &Axn_b[0], send_countsB[rank]);
    double Axn_b_p_norm = inner2(&Axn_b[0], &Axn_b[0], send_countsB[rank]);
    double Axn_b_norm;
    MPI_Allreduce(&Axn_b_p_norm, &Axn_b_norm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    double border = epsilon * epsilon * b_norm * b_norm;

    for (int i = 0; i < MAX_ITERATION && Axn_b_norm > border; ++i) {
        yn_1_p = Axn_b;
        xn_1 = x_n;
        MPI_Allgatherv(&yn_1_p[0], send_countsB[rank], MPI_DOUBLE, &yn_1[0], &send_countsB[0], &displsB[0], MPI_DOUBLE,
                       MPI_COMM_WORLD);

        matrix_vector_multiplication(A, yn_1, N, send_countsB[rank], Ayn_1);

        yAy_p = inner2(&yn_1_p[0], &Ayn_1[0], send_countsB[rank]);
        AyAy_p = inner2(&Ayn_1[0], &Ayn_1[0], send_countsB[rank]);
        MPI_Allreduce(&yAy_p, &yAy, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&AyAy_p, &AyAy, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        tn_1 = yAy / AyAy;

        vector_scalar_multiplication(yn_1_p, tn_1, send_countsB[rank], tyn_1_p);
        minusVec(&xn_1[displsB[rank]], &tyn_1_p[0], &x_n_p[0], send_countsB[rank]);
        MPI_Allgatherv(&x_n_p[0], send_countsB[rank], MPI_DOUBLE, &x_n[0], &send_countsB[0], &displsB[0], MPI_DOUBLE,
                       MPI_COMM_WORLD);

        vector_scalar_multiplication(Ayn_1, tn_1, send_countsB[rank], tAyn_1);
        minusVec(&yn_1_p[0], &tAyn_1[0], &Axn_b[0], send_countsB[rank]);
        Axn_b_p_norm = inner2(&Axn_b[0], &Axn_b[0], send_countsB[rank]);
        MPI_Allreduce(&Axn_b_p_norm, &Axn_b_norm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    }
    if (rank == 0){
        print_x(x_n);
        std::cout << "Norm: " << std::sqrt(Axn_b_norm) << std::endl;
        std::cout << "time: " << MPI_Wtime() - start << std::endl;
    }


    MPI_Finalize();
    return 0;
}
