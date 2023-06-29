//
// Created by io on 19.05.2023.
//

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include "b.h"

void b::f() {
    pid_t parent_pid;

    std::cout << "pid" << getpid() << std::endl;


    std::cout << "do fork.." << std::endl;


    pid_t pid;
    int rv;
    switch (pid = fork()) {
        case -1:
            throw std::runtime_error("can't do fork");
        case 0:
            std::cout << "CHILDREN:\tmy pid: " << getpid() << "\tparent pid: " << getppid() << std::endl;
            sleep(5);
            std::cout << "CHILDREN:\tmy pid: " << getpid() << "\tparent pid: " << getppid() << std::endl;
            sleep(5);
            exit(3);
        default:
            sleep(3);
            exit(0);



    }

}
