#include "thread.h"
#include "cpu.h"
#include <iostream>

void stub(thread_startfunc_t func, void* arg, bool *finished, std::queue<std::pair<int, ucontext_t*>> join_queue, int id){
  cpu::interrupt_enable();
  (*func)(arg);
  cpu::interrupt_disable();

  if (cpu::thread_exists.count(id) != 0){
    *finished = true;
    while (!join_queue.empty()){
      auto cxt = join_queue.front();
      cpu::ready_queue.push(cxt);
      join_queue.pop();
    }
  }
  while (!cpu::finished_queue.empty()){
    int c = cpu::finished_queue.front();
    cpu::finished_queue.pop();
    delete[] reinterpret_cast<char*>(cpu::stack_ptr[c].second);
    delete cpu::stack_ptr[c].first;
    cpu::stack_ptr.erase(c);
  }
  cpu::finished_queue.push(id);
  if(!cpu::ready_queue.empty()){
    auto chosen = cpu::ready_queue.front();
    cpu::curThread = chosen.first;
    cpu::curContext = chosen.second;
    cpu::ready_queue.pop();
    setcontext(cpu::curContext);
  } else {
    cpu::interrupt_enable_suspend();
  }
};

thread::thread(thread_startfunc_t func, void* arg){
  cpu::interrupt_disable();
  ucontext_t* context = nullptr;
  try {
    context = new ucontext_t;
    context->uc_stack.ss_sp = new char [STACK_SIZE];
  } catch (const std::bad_alloc& e){
    cpu::interrupt_enable();
    throw e;
  }
  context->uc_stack.ss_size = STACK_SIZE;
  context->uc_stack.ss_flags = 0;
  context->uc_link = nullptr;
  id = cpu::threadcount;
  cpu::stack_ptr[id] = std::make_pair(context, context->uc_stack.ss_sp);
  makecontext(context, reinterpret_cast<void (*)()>(stub), 5, func, arg, &finished, &join_queue, id);
  
  std::pair<int, ucontext_t*> current = {id, context};
  cpu::threadcount++;
  cpu::thread_exists.insert(id);
  cpu::ready_queue.push(current);
  cpu::interrupt_enable();
}

void thread::yield(){
    cpu::interrupt_disable();
    if(!cpu::ready_queue.empty()){
      std::pair<int, ucontext_t*> curPair = {cpu::curThread, cpu::curContext};
      cpu::ready_queue.push(curPair);

      auto chosen = cpu::ready_queue.front();
      cpu::ready_queue.pop();
      cpu::curThread = chosen.first;
      cpu::curContext = chosen.second;
      swapcontext(curPair.second, cpu::curContext);
    }
    else{
      // printf("no thread to yield to\n");
    }
    cpu::interrupt_enable();
}

thread::~thread(){
  cpu::thread_exists.erase(id);
}

void thread::join(){
  cpu::interrupt_disable();
  if (!finished){
    /* join itself deadlock */

    std::pair<int, ucontext_t*> curPair = {cpu::curThread, cpu::curContext};
    join_queue.push(curPair);
    
    if(!cpu::ready_queue.empty()){
      auto chosenPair = cpu::ready_queue.front();
      cpu::curThread = chosenPair.first;
      cpu::curContext = chosenPair.second;
      cpu::ready_queue.pop();
      swapcontext(curPair.second, cpu::curContext);
    } else {
      cpu::interrupt_enable_suspend();
    }
  }
  cpu::interrupt_enable();
}