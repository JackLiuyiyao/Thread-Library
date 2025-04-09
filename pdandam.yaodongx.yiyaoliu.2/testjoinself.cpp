#include "thread.h"
#include "cpu.h"
#include "mutex.h"
#include <iostream>

mutex m1;
mutex m2;
void func2(void *a){
    m1.lock();
    m2.lock();
}

void func(void *a){
    thread t1(func2, nullptr);
    m2.lock();
    thread::yield();
    m1.lock();
    std::cout << "sss";
}

int main(){
    cpu::boot(1, func, nullptr, false, false, 0);
}