#include "thread.h"
#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include <iostream>

mutex m;
mutex m2;
mutex m3;
cv waitingChild;
cv waitingParent;
int count = 5;

void child(void* a){
    m.lock();
    std::cout << "child1 lock" << std::endl;
    waitingChild.wait(m);
    m.unlock();
    std::cout << "child1 unlock" << std::endl;
}

void child2(void* a){
    m2.lock();
    std::cout << "child2 lock" << std::endl;
    while (count == 0) {
        waitingChild.wait(m2);
    }
    m2.unlock();
}

void parent(void* a){
    for (int i = 0; i < 2; ++i) {
        thread t1(child,nullptr);
        thread t2(child2,nullptr);
    }
    m2.lock();
    waitingChild.broadcast();
    std::cout << "wake\n";
    m2.unlock();
    m3.lock();
}

int main(){
  cpu::boot(1,parent,nullptr, true, true, 0);
}