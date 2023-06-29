#include <iostream>
#include <csignal>
#include <sys/ptrace.h>
#include <sys/procfs.h>
#include <sys/wait.h>



void child(std::string prog_name) {

    ptrace(PTRACE_TRACEME, 0, 0, 0);
    execl(prog_name.c_str(), nullptr);
    perror("execl");
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        return 0;

    std::string prog_name = argv[1];
    bool  *flag = new bool;
    *flag = false;
    pid_t pid = fork();
    if (pid>0) {
        *flag = true;
        int status;
        wait(&status);
    }
    else if(pid == 0) {
        sleep(5);
        std::cout << *flag;
    }
    else
        exit(1);
    return 0;
}
