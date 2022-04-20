#include "uint.h"
#include "thread.h"
#include "scheduler.h"
struct thread* run_queue = NULL, *wait_queue = NULL;

int schedule(){
    while(1){
        if(run_queue == NULL){
            set_first_thread();
        }
        struct thread* tmp = run_queue;
        run_queue = run_queue->next;
        if(tmp->status == running){
            struct thread* prev = get_current();
            switch_to(prev,tmp);
            return;
        }else{
            if(tmp->tid == 0){
                tmp->status = running;
                set_current(tmp);
                tmp->func();
            }else{
                struct thread* prev = get_current();
                tmp->status = running;
                store_and_jump(prev,tmp);
                //uart_printf("0x%x 0x%x\n",a,tmp->registers[0]);
                tmp->func();
                //rev_stack();
                tmp->status = dead;
            }
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
        run_queue = thread;
    }
}

void* push2waiting_queue(){
    struct thread *thread = get_current();
    if(wait_queue != NULL){
        struct thread *tmp = wait_queue;
        while(tmp->next != NULL) tmp = tmp->next;
        tmp->next = thread;
    }else{
        wait_queue = thread;
    }
}

void wakeup_queue(struct thread *t){
    uart_printf("789");
    /*struct thread *tmp = wait_queue;
    if(tmp != NULL){
        if(tmp->tid == t->tid){
            wait_queue = wait_queue->next;
            push2run_queue(t);
            return;
        }else{
            while(tmp->next != NULL){
                if(tmp->next->tid == t->tid){
                    tmp->next = tmp->next->next;
                    push2run_queue(t);
                    return;
                }else{
                    tmp = tmp->next;
                }
            }
        }
    }*/
}