#include "thread.h"
#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include <iostream>

//parent thread joining multiple child threads
mutex m;
cv waitingChild;
cv waitingParent;
int count = 10;

void child(void* a){
    std::cout << "child" << count << std::endl;
    m.lock();
    while (count == 0) {
        waitingChild.wait(m);
    }
    count--;
    waitingParent.signal();
    m.unlock();
}

void parent(void* a){
    m.unlock();
    std::cout << "parent" << count << std::endl;
    thread t1(child,nullptr);
    thread t2(child,nullptr);
    t1.join(); // Joining multiple child threads with the parent thread
    t2.join();
    m.lock();
    while (count == 10) {
        waitingParent.wait(m);
    }
    count++;
    waitingChild.broadcast();
    m.unlock();
}

int main(){
    cpu::boot(1,parent,nullptr, true, true, 0);
}