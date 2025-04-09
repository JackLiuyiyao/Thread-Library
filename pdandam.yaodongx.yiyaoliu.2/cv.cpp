#include "mutex.h"
#include "cpu.h"
#include "thread.h"
#include "cv.h"

// Monitor lock is held by current thread.
void cv::wait(mutex &mx) {
    cpu::interrupt_disable();
    mx.release_lock();
    std::pair<int, ucontext_t*> curPair = {cpu::curThread, cpu::curContext};
    cv_waiting_queue.push(curPair);

    // switch to next ready queue;
    if(!cpu::ready_queue.empty()){
        auto chosenPair = cpu::ready_queue.front();
        cpu::curThread = chosenPair.first;
        cpu::curContext= chosenPair.second;
        cpu::ready_queue.pop();
        swapcontext(curPair.second, cpu::curContext);
    } else {
        cpu::interrupt_enable_suspend();
    }
    cpu::interrupt_enable();
    mx.lock();
 }
 
 void cv::signal() {
    cpu::interrupt_disable();
    if (!cv_waiting_queue.empty()) {
        auto chosenPair = cv_waiting_queue.front();
        cv_waiting_queue.pop();
        cpu::ready_queue.push(chosenPair);
    }
    cpu::interrupt_enable();
 }

 void cv::broadcast() {
    cpu::interrupt_disable();
    while (!cv_waiting_queue.empty()) {
        auto chosenPair = cv_waiting_queue.front();
        cv_waiting_queue.pop();
        cpu::ready_queue.push(chosenPair);
    }
    cpu::interrupt_enable();
 }

 cv::cv(){}
 cv::~cv(){}


 // cv wait bug? testcase?
 // yield join when no thread in ready queue
 // join itself
 // error handling
 // other test cases -- for memory?
 // handgrade style
 // interrupt suspend