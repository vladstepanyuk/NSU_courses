//
// Created by io on 13.04.2023.
//

#ifndef INC_4_SOLUTION_H
#define INC_4_SOLUTION_H
#include <vector>
#include "mpi.h"
#include "MatrixUtils.h"


class Solution {
public:
    static void
    matrix_mult(int sizey, int sizex, std::vector<double> &A, std::vector<double> &B, int n1, int n2, int n3);
};


#endif //INC_4_SOLUTION_H
