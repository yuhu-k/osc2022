#include "uint.h"
#pragma once
void Thread(void *func(void));

struct thread{
    unsigned long long registers[2*7];
    void* (*func)(void);
    struct thread* next;
    int priority;
    bool have_run;
};
