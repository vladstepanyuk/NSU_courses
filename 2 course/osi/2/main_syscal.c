#include "hello.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h> /* для определений SYS_xxx */

void hello_world_without_syscall(){
    char str[13] = "Hello world\n";
    __asm__(
        "movq $1, %%rdi\n"
        "movq $13, %%rdx\n"
        "syscall\n"
        :
        :"S"(str), "a"(SYS_write)
        : "rdi", "rdx"
    );
}

int main() {
    hello_world_without_syscall();
    return 0;
}


