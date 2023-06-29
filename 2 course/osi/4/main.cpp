#include "a.h"
#include "b.h"
#include <string>


using namespace std;

int main(int argc, char **argv) {
    if (argc < 3) return 0;
    string paragraph = argv[1];
    int num = atoi(argv[2]);

    if (paragraph == "a") {
        switch (num) {
            case 1:
                a::f1();
                break;
            case 2:
                a::f2();
                break;
            case 3:
                a::f3();
                break;
            case 4:
                a::f4();
                break;
            default:
                return 0;
        }
    } else if (paragraph == "b") {
        switch (num) {
            case 1:
                b::f1();
                break;
            case 2:
                b::f2();
                break;
            case 3:
                b::f3();
                break;
            case 4:
                b::f4();
                break;
            default:
                return 0;
        }
    }


    return 0;
}


