#include "hello.h"
#include <dlfcn.h>
#include <unistd.h>


void hello() {
    printf("Hello world\n");
}

int main() {
    hello();
    return 0;
}


