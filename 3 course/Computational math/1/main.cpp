#include <iostream>
#include "Solver.h"


#include <limits>


int main() {

    try {
        comp_math::Solver solver(1e-8, 10);

        auto answer = solver.solve_cube_equation(-3.942, -5.29,0);
        for (auto x: answer) {
            std::cout << x << ' ';
        }

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
