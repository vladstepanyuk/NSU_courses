//
// Created by 1 on 05.09.2023.
//

#include <cmath>
#include <string>
#include <stdexcept>
#include <limits>
#include "Solver.h"

namespace {
    double DOUBLE_INFINITY = std::numeric_limits<double>::infinity();
}

double comp_math::Solver::solve_monotonic_function(const std::function<double(double)> &function, double a, double b) {
    if (b <= a || (a == -DOUBLE_INFINITY && b == DOUBLE_INFINITY))
        throw std::invalid_argument("invalid arguments");

    if (b == DOUBLE_INFINITY)
        return solve_monotonic_function_on_endless_segment(function, a, 1);
    if (a == -DOUBLE_INFINITY)
        return solve_monotonic_function_on_endless_segment(function, b, -1);

    return solve_monotonic_function_on_final_segment(function, a, b);
}

double
comp_math::Solver::solve_monotonic_function_on_final_segment(const std::function<double(double)> &function, double a,
                                                             double b) const {
    if (is_zero_weak(function(a)) && is_zero_weak(function(b)))
        return (a + b) / 2;
    if (std::abs(function(a)) < epsilon)
        return a;
    if (std::abs(function(b)) < epsilon)
        return b;


    std::function<double(double)> new_function;
    if (function(a) > epsilon)
        new_function = [function](double x) { return -1 * function(x); };
    else
        new_function = function;

    if (new_function(b) < -epsilon)
        throw std::invalid_argument("no roots");

    double c = (a + b) / 2;

    for (int i = 0; i < N && !is_zero_weak(new_function(c)); ++i) {

        if (new_function(c) > epsilon)
            b = c;
        else if (new_function(c) < -epsilon)
            a = c;

        c = (a + b) / 2;
    }


    if (is_zero_weak(function(c)))
        return c;
    else
        throw std::invalid_argument(std::string("can't find root with N == ") + std::to_string(N));

}

double
comp_math::Solver::solve_monotonic_function_on_endless_segment(const std::function<double(double)> &function,
                                                               double start_point,
                                                               int direction) {

    if (is_zero_weak(function(start_point)))
        return start_point;

    std::function<double(double)> new_function;
    if (function(start_point) > epsilon)
        new_function = [function](double x) { return -1 * function(x); };
    else
        new_function = function;


    double point_cur = start_point + direction * delta;
    double point_prev = start_point;

    for (int i = 0; i < N && new_function(point_cur) < -epsilon; ++i) {
        point_prev = point_cur;
        point_cur = point_prev + direction * delta;
    }


    if (is_zero_weak(function(point_cur)))
        return point_cur;
    if (new_function(point_cur) < -epsilon)
        throw std::invalid_argument(std::string("can't find root with N == ") + std::to_string(N));

    return solve_monotonic_function_on_final_segment(function, std::min(point_cur, point_prev),
                                                     std::max(point_cur, point_prev));

}

comp_math::Solver::Solver(double epsilon, double delta, int n, double zero) : epsilon(epsilon), delta(delta),
                                                                              zero(zero), N(n) {}

std::vector<double> comp_math::Solver::solve_cube_equation(double a, double b, double c) {
    std::vector<double> answer;

    auto function = [a, b, c](double x) {
        return x * x * x + a * x * x + b * x + c;
    };

    double desc = 4 * a * a - 12 * b;
    if (desc <= 12 * zero && is_zero_weak(function(0)))
        return {0};

    if (desc <= 12 * zero && function(0) > epsilon)
        return {solve_monotonic_function(function, -DOUBLE_INFINITY, 0)};

    if (desc <= 12 * zero && function(0) < -epsilon)
        return {solve_monotonic_function(function, 0, DOUBLE_INFINITY)};

    double x2 = (-2 * a + std::sqrt(desc)) / (2 * 3);
    double x1 = (-2 * a - std::sqrt(desc)) / (2 * 3);

    if (function(x1) > epsilon && function(x2) > epsilon)
        return {solve_monotonic_function_on_endless_segment(function, x1, -1)};
    else if (function(x1) < -epsilon && function(x2) < -epsilon)
        return {solve_monotonic_function_on_endless_segment(function, x2, 1)};
    else if (function(x1) > epsilon && is_zero_weak(function(x2)))
        return {x2, solve_monotonic_function_on_endless_segment(function, a, -1)};
    else if (is_zero_weak(function(x1)) && function(x2) < -epsilon)
        return {a, solve_monotonic_function_on_endless_segment(function, x2, 1)};
    else if (function(x1) > epsilon && function(x2) < -epsilon)
        return {solve_monotonic_function_on_endless_segment(function, x1, -1),
                solve_monotonic_function_on_final_segment(function, x1, x2),
                solve_monotonic_function_on_endless_segment(function, x2, 1)};
    else if (is_zero_weak(x1) && is_zero_weak(x2))
        return {(x1 + x2) / 2};


}

bool comp_math::Solver::is_zero_weak(double x) const {
    return std::abs(x) < epsilon;
}

