//
// Created by 1 on 05.09.2023.
//

#ifndef INC_1_SOLVER_H
#define INC_1_SOLVER_H


#include <functional>
#include <vector>


namespace comp_math {
    class Solver {

    private:
        double epsilon, delta;
        double zero;
        int N;

        double solve_monotonic_function_on_final_segment(const std::function<double(double)>& function, double a, double b) const;
        double solve_monotonic_function_on_endless_segment(const std::function<double(double)>& function, double start_point, int direction);
        bool is_zero_weak(double x) const;
    public:

        Solver(double epsilon, double delta,  int n = 10000, double zero = 1e-8);

        std::vector<double> solve_cube_equation(double a, double b, double c);

        double solve_monotonic_function(const std::function<double(double)>& function, double a, double b);


    };


}


#endif //INC_1_SOLVER_H
