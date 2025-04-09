#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;

mutex mutex1;
cv cv1;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    cout << "loop called with id " << id << endl;

    for (i=0; i<5; i++, g++) {
        mutex1.unlock();
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex1.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    thread::yield();
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 (loop, static_cast<void *>(const_cast<char *>("child thread1")));
    thread t2 (loop, static_cast<void *>(const_cast<char *>("child thread2")));
    // thread t3 (loop, static_cast<void *>(const_cast<char *>("child thread3")));
    t1.join();
    t1.join();
    t1.join();
    t1.join();
    t2.join();
    // t3.join();
    loop(static_cast<void *>(const_cast<char *>("parent thread")));
    cout << "parent finished\n";
}

int main()
{
    cpu::boot(1, parent, nullptr, true, true, 0);
}