#include "c.h"
#include <iostream>
#include "Utils.h"


typedef long long ll;


int main() {
    try {
        printf("pid: %d\n", getpid());
        c::mapNewSection();

        char *myString = (char *) c::malloc(sizeof(char) * 20 * 1024);


        char *myString2 = (char *) c::malloc(sizeof(char) * 20 * 1024);


        std::string hw = "hello world";


        for (int i = 0; i < hw.size(); ++i) {
            myString[i] = hw[i];
        }

        for (int i = 0; i < hw.size(); ++i) {
            std::cout << myString[i];
        }


        std::cout << std::endl;

        try {
            int *a = (int *) c::malloc(sizeof(int) * 4);
        } catch (std::exception &e){
            std::cerr << e.what() << std::endl;
        }


        c::free(myString+50);



        int *a = (int *) c::malloc(sizeof(int) * 4);

        a[0] = 1;
        a[1] = 2;
        a[2] = 3;
        a[3] = 4;

        std::cout << (ll)myString2 - (ll)a;
    } catch (std::exception &e){
        std::cerr << e.what() << std::endl;
    }



}