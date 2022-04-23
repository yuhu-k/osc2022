#include "uint.h"
#include "thread.h"
#include "scheduler.h"
struct thread* run_queue = NULL, *wait_queue = NULL;

int schedule(){
    if(run_queue == NULL){
        push_first_thread();
    }
    struct thread* prev = get_current();
    struct thread* tmp = run_queue;
    run_queue = run_queue->next;
    void* sp_addr,*pc_addr;
    if(tmp->status == running){
        switch_to(prev,tmp);
    }else{
        tmp->status = running;
        if(tmp->tid == 0){
            set_current(tmp);
        }else{
            store_and_jump(prev,tmp);
            return;
        }
    }
}

void push2run_queue(struct thread* thread){
    if(run_queue != NULL){
        struct thread *tmp = run_queue;
        while(tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = thread;
        thread->next = NULL;
    }else{
        thread->next = NULL;
        run_queue = thread;
    }
}

void wakeup_queue(struct thread *t){
    /*struct thread *tmp = wait_queue;
    if(tmp != NULL){
        if(tmp->tid == t->tid){
            wait_queue = wait_queue->next;
            t->status = running;
            push2run_queue(t);
            return;
        }else{
            while(tmp->next != NULL){
                if(tmp->next->tid == t->tid){
                    tmp->next = tmp->next->next;
                    t->status = running;
                    push2run_queue(t);
                    return;
                }else{
                    tmp = tmp->next;
                }
            }
        }
    }*/
    t->status = running;
    push2run_queue(t);
}

void p(struct thread *t)
{
    t->status = dead;
};
