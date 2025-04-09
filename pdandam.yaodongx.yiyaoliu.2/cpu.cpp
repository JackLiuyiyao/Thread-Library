#include "cpu.h"
#include "thread.h"
#include <iostream>

std::queue<std::pair<int, ucontext_t*>> cpu::ready_queue;
std::unordered_set<int> cpu::thread_exists;
std::unordered_map<int, std::pair<ucontext_t*, void*>> cpu::stack_ptr;
int cpu::curThread = -1;
int cpu::threadcount = 0;
ucontext_t* cpu::cpuContext = new ucontext_t;
ucontext_t* cpu::curContext = new ucontext_t;
std::queue<int> cpu::finished_queue;

void cpu::timer_handler(){
    // printf("begin interrupt %d\n", curThread);
    cpu::interrupt_disable();
    // printf("disabled in timer");
    if (curThread != -1){
        std::pair<int, ucontext_t*> curPair = {cpu::curThread, cpu::curContext};
        ready_queue.push(curPair);
        curThread = -1;
        curContext = cpuContext;
        swapcontext(curPair.second, curContext);
    }
    cpu::interrupt_enable();
}

cpu::cpu(thread_startfunc_t func, void* arg){
    cpu::interrupt_vector_table[cpu::TIMER] = &timer_handler;
    cpu::interrupt_vector_table[cpu::IPI] = &IPI_handler;
    getcontext(cpuContext);
    
    cpu::interrupt_enable();
    thread t1(func, arg);
    cpu::interrupt_disable();

    while(true){
        if (!ready_queue.empty()){
            auto current = ready_queue.front();
            curThread = current.first;
            curContext = current.second;
            ready_queue.pop();
            swapcontext(cpuContext, curContext);
        }
        // else {
        //     cpu::interrupt_enable_suspend();
        // }
    }
    cpu::interrupt_enable_suspend();
}

void cpu::IPI_handler(){}