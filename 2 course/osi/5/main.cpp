#include <iostream>
#include "a.h"
#include "b.h"




int main(int argc, char **argv) {

    if (argc < 2) return 0;

    std::string point(argv[1]);

    if (point == "a") {
        a::f();
    } else if (point == "b") {
        b::f();
    }



    return 0;
}
