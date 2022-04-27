#include "thread.h"
#include "signal.h"
#include "scheduler.h"
#include "uint.h"

extern struct thread *threads[thread_numbers];

int signal(int SIGNAL, void (*handler)()){
    struct thread *t = get_current();
    t->sig_handler[SIGNAL] = handler;
}

int killpid(int pid, int SIGNAL){
    threads[pid]->signal |= 1<<SIGNAL;
}

void sig_handler_kernel(struct thread *t){
    for(int i=0;i<32;i++){
        if((t->signal & 1<<i) && t->sig_handler[i] != NULL){
            void (*handler)(unsigned int pid) = t->sig_handler[i];
            handler(t->tid);
            t->signal &= !(1<<i);
        }
    }
}