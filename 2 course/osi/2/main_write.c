#include "hello.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h> /* для определений SYS_xxx */
void hello() {
    syscall(SYS_write, 1, "Hello world\n", 12); 
}

int main(){
    hello();
    return 0;
}


