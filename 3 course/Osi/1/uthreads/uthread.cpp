#include "uthread.h"
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>


std::vector<uthread_t> uthreads;
std::mutex list_guard;

//auto i = 0;
uthread_t cur_thread;

const int PAGE_SIZE = 4096;
const int STACK_SIZE = PAGE_SIZE * 8;
char SCHEDULER_STACK[STACK_SIZE];
ucontext_t scheduler_thread_ctx;
bool is_sched_init = false;
std::thread scheduler_thread;

int findRunningThread(int start) {
    for (int i = 0; i < uthreads.size(); ++i) {
        if (!uthreads[(start + i) % uthreads.size()]->is_ended) {
            return (start + i) % uthreads.size();
        }
    }

    return -1;
}


void reschedule() {
    list_guard.lock();
    int i = cur_thread->i;
    int nextI = findRunningThread((i + 1) % uthreads.size());
    if (nextI == i || nextI == -1) {
        list_guard.unlock();
        return;
    }
    auto next_thread = uthreads[nextI];
    list_guard.unlock();


    auto *tmp = cur_thread;
    cur_thread = next_thread;


    swapcontext(&tmp->context, &next_thread->context);
}

void shedulerFunc() {
    int i;
    while (true) {
        list_guard.lock();
        i = findRunningThread(0);
        if (i != -1)
            break;
        list_guard.unlock();

        sleep(1);
    }


    ucontext_t ctx;

    getcontext(&ctx);

    auto *tmp = uthreads[i];

    list_guard.unlock();

    cur_thread = tmp;

    swapcontext(&ctx, &tmp->context);
}


void wrapped_func(uthread_struct_t *uthread_struct) {
    void *result = uthread_struct->func(uthread_struct->arg);


    uthread_struct->result = result;
    uthread_struct->is_ended = true;
    //    while (!uthread_struct->is_joined) {
    //        sleep(1);
    //    }
}

int uthread_create(uthread_t &tid, start_routine_t func, void *arg) {
    static int thread_num = 0;
    auto *uthread =
            (uthread_struct_t *) malloc(sizeof(uthread_struct_t));
    if (!uthread)
        return -1;
    thread_num++;

    if (!is_sched_init) {
        is_sched_init = 1;
        getcontext(&scheduler_thread_ctx);
        scheduler_thread_ctx.uc_stack.ss_size = STACK_SIZE;
        scheduler_thread_ctx.uc_stack.ss_sp = SCHEDULER_STACK;
        makecontext(&scheduler_thread_ctx, shedulerFunc, 0);

        scheduler_thread = std::thread(shedulerFunc);
        scheduler_thread.detach();
    }


    uthread->func = func;
    uthread->arg = arg;
    uthread->id = thread_num;
    uthread->is_ended = false;
//    uthread->is_joined = false;
    void *stack = malloc(STACK_SIZE);
    if (!stack) {
        free(uthread);
        return -1;
    }
    //    std::cout << stack << std::endl;

    uthread->stack = stack;

    getcontext(&uthread->context);
    uthread->context.uc_stack.ss_sp = stack;
    uthread->context.uc_stack.ss_size = STACK_SIZE;
    uthread->context.uc_link = &scheduler_thread_ctx;
    makecontext(&uthread->context, reinterpret_cast<void (*)()>(wrapped_func), 1, uthread);

    tid = uthread;


    list_guard.lock();
    uthread->i = uthreads.size();
    uthreads.emplace_back(tid);
    list_guard.unlock();


    return 0;
}

void uthread_join(uthread_t tid, void **result) {
    uthread_struct_t *uthread = tid;

    while (!uthread->is_ended) {
        sleep(1);
    }

    free(uthread->stack);

    if (result != nullptr)
        *result = uthread->result;
    list_guard.lock();


    std::swap(uthreads[uthread->i], uthreads[uthreads.size() - 1]);

    uthreads[uthread->i]->i = uthread->i;

    uthreads.pop_back();
    list_guard.unlock();

    free(uthread);

}