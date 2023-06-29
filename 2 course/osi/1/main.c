//#include <stdio.h>
#include "hello.h"
#include <dlfcn.h>


void hello() {
    printf("Hello world\n");
}

int main() {
    hello();
    hello_from_static_lib();
    void *lib;
    void (*hello_dynamic)();
    lib = dlopen("/home/russian/Osi/1/libmylib_dynamic.so", RTLD_LAZY);
    if (!lib) {
        fputs (dlerror(), stderr);
        return 1;
    }
    hello_dynamic = dlsym(lib, "hello_from_dynamic_lib");
    hello_dynamic();

    dlclose(lib);
    return 0;
}
