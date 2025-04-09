#include "thread.h"
#include "cpu.h"
#include "mutex.h"
#include <iostream>

mutex m;
void func2(void* a){
  m.lock();
  thread::yield();
  m.unlock();
}
void testFunc(void* a){
  thread t(func2, nullptr);
  thread::yield();
  try {m.unlock();}
  catch (...){
    std::cout << "don't acquire other's lock" << std::endl;
  }
  m.lock();
  m.unlock();
}

int main(){
  
    cpu::boot(1,testFunc,reinterpret_cast<void *>(100), false, false, 0);
}