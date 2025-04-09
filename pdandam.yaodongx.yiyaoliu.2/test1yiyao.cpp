#include "thread.h"
#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include <iostream>

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

void child2(void* a){
    std::cout << "child2" << count << std::endl;
    m.lock();
    while (count > 0) {
        waitingChild.wait(m);
    }
    std::cout << "child2 exiting" << std::endl;
    m.unlock();
}

void parent2(void* a){
    std::cout << "parent2" << count << std::endl;
    int max = 10;
    for (int i = 0; i < 5; ++i) {
        thread t1(child2,nullptr);
        t1.join(); // Joining a thread with itself
    }
    m.lock();
    count = 0;
    waitingChild.broadcast();
    m.unlock();
}

void parent(void* a){
    thread p2(parent2,nullptr);
    std::cout << "parent" << count << std::endl;
    int max = 10;
    for (int i = 0; i < 5; ++i) {
        thread t1(child,nullptr);
        t1.join(); // Joining a thread with itself
    }
    m.lock();
    while (count == max) {
        waitingParent.wait(m);
    }
    count++;
    waitingChild.broadcast();
    m.unlock();
}

int main(){
    cpu::boot(1,parent,nullptr, true, true, 0);
}
