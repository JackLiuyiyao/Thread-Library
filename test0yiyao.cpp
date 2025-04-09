#include "thread.h"
#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include <iostream>

mutex m1;
mutex m2;
cv cv1;
cv cv2;
int count1 = 5;
int count2 = 10;

void child1(void* a){
    m1.lock();
    while (count1 == 0) {
        cv1.wait(m1);
    }
    std::cout << "child1: " << count1 << std::endl;
    count1--;
    cv1.signal();
    m1.unlock();
}

void child2(void* a){
    m2.lock();
    while (count2 == 0) {
        cv2.wait(m2);
    }
    std::cout << "child2: " << count2 << std::endl;
    count2--;
    cv2.signal();
    m2.unlock();
}

void parent(void* a){
    std::cout << "parent" << std::endl;
    int max1 = 5;
    int max2 = 10;
    for (int i = 0; i < 5; ++i) {
        thread t1(child1,nullptr);
        thread t2(child2,nullptr);
        t1.join();
        t2.join();
    }
    m1.lock();
    while (count1 == max1) {
        cv1.wait(m1);
    }
    count1++;
    cv1.broadcast();
    m1.unlock();
    m2.lock();
    while (count2 == max2) {
        cv2.wait(m2);
    }
    count2++;
    cv2.broadcast();
    m2.unlock();
}

int main(){
    cpu::boot(1,parent,nullptr, true, true, 0);
}
