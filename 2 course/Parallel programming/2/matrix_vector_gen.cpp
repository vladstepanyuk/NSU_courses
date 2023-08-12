#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

using namespace std;

void matrix_gen(int N, ofstream &file) {
    srand(time(NULL));
    double random = 0;
    file.setf(ios::fixed);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            random = (2 * (static_cast<double>(rand()) / RAND_MAX) - 1);
            if (i == j) random += N;
            
            file<<setprecision(2)<< random <<' ';
        }
        file << endl;
    }
}

void vector_gen(int N, ofstream &file) {
    srand(time(NULL));
    file.setf(ios::fixed);
    double random = 0;
    for (int i = 0; i < N; ++i) {
        random = (2 * (static_cast<double>(rand()) / RAND_MAX) - 1);
	file<<setprecision(2)<< random <<' ';

    }
}

int main(int argc, char **argv) {
    if (argc < 4) return 0;
    int N = atoi(argv[1]);
    ofstream file_matrix(argv[2]);
    ofstream file_b_vector(argv[3]);
    matrix_gen(N, file_matrix);
    vector_gen(N, file_b_vector);
    return 0;
}
