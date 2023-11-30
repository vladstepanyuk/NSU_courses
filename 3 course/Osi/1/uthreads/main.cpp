#include <unistd.h>
#include <sys/types.h>
#include "uthread.h"
#include <iostream>

void *f1(void *) {
    while (true) {
        printf("[%d] f1\n", gettid());

        reschedule();
    }
}

void *f2(void *) {
    printf("[%d] f2\n", gettid());
    sleep(1);
    reschedule();
    return (void*)1;
}



void *f3(void *) {
    while (true) {
        printf("[%d] f3\n", gettid());
        sleep(1);
        reschedule();
    }
}


int main() {
    ucontext_t ctx;
    getcontext(&ctx);

    uthread_t tid1, tid2, tid3;
    uthread_create(tid1, f1, nullptr);
    uthread_create(tid2, f2, nullptr);
    uthread_create(tid3, f3, nullptr);

    size_t result;

    uthread_join(tid2, (void **)&result);
    std::cout << result << std::endl;
    uthread_join(tid3, nullptr);
    uthread_join(tid1, nullptr);

    //    swapcontext(&ctx, &uthreadStruct->context);
    sleep(10000);
    return 0;
}
