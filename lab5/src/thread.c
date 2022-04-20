#include "uint.h"
#include "thread.h"
#include "allocator.h"
#include "queue.h"
#include "scheduler.h"
#include "shell.h"

struct thread *threads[thread_numbers];

void Thread(void *func(void)){
    struct thread *t;
    t = malloc(sizeof(struct thread));
    t->func = func;
    t->next = NULL;
    t->status = starting;
    t->childs = NULL;
    t->registers[0] = func;
    push2run_queue(t);
    struct thread *temp = get_current();
    t->ptid = temp->tid;
    struct thread_sibling *temp2 = temp->childs;
    struct thread_sibling *new_child = malloc(sizeof(struct thread_sibling));
    new_child->self = t;
    new_child->next = NULL;
    if(temp2 == NULL){
        temp->childs = new_child;
    }else{
        while(temp2->next != NULL){
            temp2 = temp2->next;
        }
        temp2->next = new_child;
    }
    for(int i=0;i<thread_numbers;i++){
        if(threads[i] == NULL){
            threads[i] = t;
            t->tid = i;
            break;
        }
    }
    return ;
}

void set_first_thread(){
    threads[0]->next = NULL;
    push2run_queue(threads[0]);
}

void init_thread(){
    for(int i=0;i<thread_numbers;i++) threads[i] = NULL;

    struct thread *t;
    t = malloc(sizeof(struct thread));
    t->func = idle;
    t->status = starting;
    t->childs = NULL;
    t->ptid = 0;
    t->tid = 0;
    threads[0] = t;
    set_first_thread();
    return ;
}

void kill_zombies(){
    for(int i=1;i<thread_numbers;i++){
        if(threads[i] != NULL && threads[i]->status == dead){
            free(threads[i]);
            threads[i] = NULL;
        }
    }
}

void idle(){
    while(1){
        kill_zombies();
        Thread(uart_read_line);
        schedule();
    }
}
