#include "uint.h"
#pragma once
void Thread(void *func(void));
void idle();
void init_thread();
void set_first_thread();

#define thread_numbers 65536

struct thread_sibling{
    struct thread *self;
    struct thread_sibling *next;
};

struct thread{
    unsigned long long registers[2*7];
    void* (*func)(void);
    struct thread* next;
    int priority;
    tid_t tid, ptid;
    enum STATUS{
        starting,
        running,
        stop,
        dead
    } status;
    struct thread_sibling *childs;
    unsigned char stack[256];
};
