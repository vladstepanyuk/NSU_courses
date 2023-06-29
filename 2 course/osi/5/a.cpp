//
// Created by io on 18.05.2023.
//

#include <csignal>
#include <iostream>
#include <stdexcept>
#include "Utils.h"
#include <sys/wait.h>
//#include <wait.h>
#include "a.h"

int global_var;

void a::f() {
    int local_var = 0;
    global_var = 1;
    pid_t parent_pid;


    std::cout << "global_var\taddress: " << (void *) &global_var << "\tvalue: " << global_var << std::endl;
    std::cout << "local_var\taddress: " << (void *) &local_var << "\tvalue: " << local_var << std::endl;
    parent_pid = getpid();
    std::cout << "pid" << parent_pid << std::endl;

    Utils::type_to_continue();


    std::cout << "do fork.." << std::endl;


    pid_t pid;
    int rv;
    switch (pid = fork()) {
        case -1:
            throw std::runtime_error("can't do fork");
        case 0:
            std::cout << "CHILDREN:\tmy pid: " << getpid() << "\tparent pid: " << getppid() << std::endl;
            std::cout << "CHILDREN:\tglobal_var\taddress: " << (void *) &global_var << "\tvalue: " << global_var << std::endl;
            std::cout << "CHILDREN:\tlocal_var\taddress: " << (void *) &local_var << "\tvalue: " << local_var << std::endl;

            local_var = 12123;
            global_var = 231;


            std::cout << "CHILD:\tglobal_var\taddress: " << (void *) &global_var << "\tvalue: " << global_var << std::endl;
            std::cout << "CHILD:\tlocal_var\taddress: " << (void *) &local_var << "\tvalue: " << local_var << std::endl;
            sleep(2);
            exit(5);

        default:
            sleep(2);
            std::cout << "PARENT:\tglobal_var\taddress: " << (void *) &global_var << "\tvalue: " << global_var << std::endl;
            std::cout << "PARENT:\tlocal_var\taddress: " << (void *) &local_var << "\tvalue: " << local_var << std::endl;
            sleep(30);
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                std::cout << "PARENT: children exit code " << WEXITSTATUS(status) << std::endl;
            } else if (WIFSIGNALED(status)) {
                std::cout << "PARENT: children don't handle signal " << WTERMSIG(status) << std::endl;
            }
    }

}
