//
// Created by io on 21.04.2023.
//
#include <iostream>
#include <thread>
#include <csetjmp>
#include <csignal>
#include "b.h"

jmp_buf env;

namespace {
    void sig_handler(int signo) {
        std::cerr << "\ncatch SIGSEGV\n";
        longjmp(env, 1);
    }
}

typedef unsigned long long ull;


void b::f1() {
    pid_t pid = getpid();
    printf("The process id: %d\n", pid);
    sleep(1);
    execl("4", "4", "b", "1");
    std::cout << "hello world";

}

void b::f2() {
    pid_t pid = getpid();
    printf("The process id: %d\n", pid);
    static bool flag = false;
    if (!flag) {
        Utils::type_to_continue();
        flag = true;
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    char array[4096];

    f2();
}

void b::f3() {
    char *a[500];


    printf("The process id: %d\n", getpid());
    Utils::type_to_continue();

    for (auto &i: a) {
        i = (char *) malloc(4096 * 10);
        sleep(1);
    }


    for (auto &i: a) {
        free(i);
    }
}

void b::f4() {

    printf("The process id: %d\n", getpid());


    Utils::type_to_continue();


    void *addr = mmap(nullptr, 4096 * 10, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED)
        exit(EXIT_FAILURE);


    Utils::type_to_continue();

    addr = mmap(addr, 4096 * 10, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
    if (addr == MAP_FAILED)
        exit(EXIT_FAILURE);


    Utils::type_to_continue();


    if (setjmp (env) == 0) {
        std::signal(SIGSEGV, &sig_handler);
        *(int *) addr = 6;
        signal(SIGSEGV, SIG_DFL);
    }

    Utils::type_to_continue();

    addr = mmap(addr, 4096 * 10, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
    if (addr == MAP_FAILED)
        exit(EXIT_FAILURE);


    Utils::type_to_continue();


    if (setjmp (env) == 0) {
        std::signal(SIGSEGV, &sig_handler);
        int i = *(int *) addr;
        std::cout << i;
        signal(SIGSEGV, SIG_DFL);
    }

    Utils::type_to_continue();


    if (munmap((void *) ((ull) addr + 4096 * 4), 4096 * 2))
        exit(EXIT_FAILURE);


    Utils::type_to_continue();

}
