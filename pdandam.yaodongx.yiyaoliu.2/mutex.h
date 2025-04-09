/*
 * mutex.h -- interface to the mutex class
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#include <ucontext.h>
#include <iostream>
#include "cpu.h"
#include "thread.h"
class cpu;

class mutex {
public:
    mutex();
    ~mutex();

    void lock();
    void unlock();

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    mutex(mutex&&);
    mutex& operator=(mutex&&);

    void release_lock();

private:
    bool busy = false;
    std::queue<std::pair<int, ucontext_t*>> waiting_queue;
    int owner = -1;
};

