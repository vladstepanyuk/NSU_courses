#include "hello.h"
#include <dlfcn.h>
#include <unistd.h>


void hello() {
    printf("Hello world\n");
}

int main() {
    hello();
    hello_from_static_lib();
    hello_from_dynamic_lib();
    return 0;
}


