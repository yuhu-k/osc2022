#include "uint.h"
#include "thread.h"
#include "scheduler.h"
struct thread* run_queue = NULL, *wait_queue = NULL;
int schedule(){
    while(run_queue != NULL){
        struct thread* tmp = run_queue;
        run_queue = run_queue->next;
        if(tmp->have_run){
            struct thread* prev = get_current();
            push2run_queue(prev);
            switch_to(prev,tmp);
        }else{
            set_current(tmp);
            tmp->have_run = true;
            tmp = tmp->func();
        }
    }
}

void push2run_queue(struct thread* thread){
    if(run_queue != NULL){
        struct thread *tmp = run_queue;
        while(tmp->next != NULL) tmp = tmp->next;
        tmp->next = thread;
    }else{
        run_queue = thread;
    }
}