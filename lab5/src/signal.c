#include "thread.h"
#include "signal.h"
#include "scheduler.h"
#include "uint.h"
#include "queue.h"
#include "excep.h"

extern struct thread *threads[thread_numbers];

int signal(int SIGNAL, void (*handler)()){
    struct thread *t = get_current();
    t->sig_handler[SIGNAL] = handler;
}

int killpid(int pid, int SIGNAL){
    threads[pid]->signal |= 1<<SIGNAL;
}

void* sig_handler_kernel(struct thread *t){
    for(int i=0;i<32;i++){
        if((t->signal & 1<<i) && t->sig_handler[i] != NULL){
            int tid = Thread(from_el1_to_el0,t->sig_handler[i]);
            threads[tid]->registers[3] = t->tid;
            /*int tid = Thread(t->sig_handler[i]);
            threads[tid]->registers[2] = t->tid;*/
            t->signal &= !(1<<i);
        }
    }
    return t;
}

void q(void * addr){
    uart_printf("0x%x\n",addr);
}