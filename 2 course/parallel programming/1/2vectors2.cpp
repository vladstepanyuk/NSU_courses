#include "mpi.h"
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

int vectorMult(int *vector1, int *vector2, int vector1Size, int vector2Size) {
    int s = 0;
    for (int i = 0; i < vector1Size; ++i) {
        for (int j = 0; j < vector2Size; ++j) {
            s += vector1[i] * vector2[j];
        }
    }
    return s;
}


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc < 2) {
        MPI_Finalize();
        return (0);
    }

    int N = atoi(argv[1]);
    int NActual = N % size == 0 ? N : N + size - N % size;

    vector<int> vector1;

    vector<int> vector2(NActual);
    if (rank == 0) {

        vector1 = vector<int>(NActual, 1);
        vector2 = vector<int>(NActual, 1);

        for (int i = N; i < NActual; ++i)
            vector1[i] = vector2[i] = 0;

        MPI_Scatter(&vector1[0], NActual/size, MPI_INT, MPI_IN_PLACE, NActual/size, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        vector1 = vector<int>(NActual / size);
        MPI_Scatter(&vector1[0], NActual/size, MPI_INT, &vector1[0], NActual/size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Bcast(&vector2[0], NActual, MPI_INT, 0, MPI_COMM_WORLD);
    int s = vectorMult(&vector1[0], &vector2[0], NActual / size, NActual);
    int sum;
    MPI_Reduce(&s, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) cout << sum << endl;


    MPI_Finalize();
    return (0);
}
