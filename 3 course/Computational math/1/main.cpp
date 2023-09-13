#include <iostream>
#include "Solver.h"


#include <limits>

using namespace std;
using namespace comp_math;

int main() {

    try {

        int N;
        cin >> N;
        for (int i = 0; i < N; ++i) {
            double epsilon = 1, delta, a, b, c;
            cin >> a >> b >> c >> delta;
            Solver solver(epsilon, delta);

            auto answer = solver.solve_cube_equation(a, b, c);
            auto function = [a, b, c](double x) {
                return x * x * x + a * x * x + b * x + c;
            };

            cout << answer.size() << " roots: ";
            for (auto x: answer) {
                std::cout <<"x = " << x << " f(x) = " << abs(function(x)) << ", ";
            }

            cout << endl;
        }


    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
