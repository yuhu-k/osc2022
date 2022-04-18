#include "uint.h"
#include "thread.h"
#include "allocator.h"
#include "queue.h"

void Thread(void *func(void)){
    struct thread *t;
    t = malloc(sizeof(struct thread));
    t->func = func;
    t->registers[11] = func;
    t->next = NULL;
    t->have_run = false;
    push2run_queue(t);
    return ;
}
