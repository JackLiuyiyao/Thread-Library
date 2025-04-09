#include "mutex.h"
#include "cpu.h"
#include "thread.h"

void mutex::lock(){
    cpu::interrupt_disable();
    if (busy){
        std::pair<int, ucontext_t*> curPair = {cpu::curThread, cpu::curContext};
        waiting_queue.push(curPair);

        // switch to next ready queue;
        if(!cpu::ready_queue.empty()){
            auto chosenPair = cpu::ready_queue.front();
            cpu::curThread = chosenPair.first;
            cpu::curContext = chosenPair.second;
            cpu::ready_queue.pop();
            swapcontext(curPair.second, cpu::curContext);
        } else {
            cpu::interrupt_enable_suspend();
        }
    } else {
        /* add ownership of the lock*/
        owner = cpu::curThread;
        busy = true;
    }
    cpu::interrupt_enable();
}

void mutex::unlock(){
    cpu::interrupt_disable();
    release_lock();
    cpu::interrupt_enable();
}

void mutex::release_lock(){
    assert_interrupts_disabled();
    if (owner != cpu::curThread) {
        cpu::interrupt_enable();
        throw std::runtime_error("Mutex unlock error");
    }
    busy = false;
    owner = -1; // no one owns lock
    if (!waiting_queue.empty()) {
        auto chosenPair = waiting_queue.front();
        waiting_queue.pop();
        cpu::ready_queue.push(chosenPair);
        owner = chosenPair.first;
        busy = true;
    }
}

mutex::mutex(){}

mutex::~mutex(){}