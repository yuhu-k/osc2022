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
    delete_last_mem();
    t->next = NULL;
    t->status = starting;
    t->childs = NULL;
    t->registers[0] = func;
    t->registers[1] = ( (uint64)(t->stack + 0x10000) & 0xfffffff0);
    struct thread *temp = get_current();
    t->ptid = temp->tid;
    t->malloc_table[0] = NULL;
    struct thread_sibling *temp2 = temp->childs;
    struct thread_sibling *new_child = malloc(sizeof(struct thread_sibling));
    delete_last_mem();
    void *a;
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
    push2run_queue(t);
    return ;
}


void set_first_thread(){
    struct thread *t;
    t = malloc(sizeof(struct thread));
    delete_last_mem();
    t->status = starting;
    t->childs = NULL;
    t->ptid = 0;
    t->tid = 0;
    threads[0] = t;
    t->malloc_table[0] = NULL;
    t->next = NULL;
    t->registers[0] = idle;
    t->registers[1] = ( (uint64)(t->stack + 0x10000) & 0xfffffff0);
    push2run_queue(t);
}

void push_first_thread(){
    threads[0]->next = NULL;
    push2run_queue(threads[0]);
}

void init_thread(){
    clear_threads();
    return ;
}

void kill_zombies(){
    for(int i=1;i<thread_numbers;i++){
        if(threads[i] != NULL && threads[i]->status == dead){
            free_mem_table(threads[i]);
            free(threads[i]);
            threads[i] = NULL;
        }
    }
}

void idle(){
    while(1){
        handle_child(0);
        kill_zombies();
        Thread(uart_read_line);
        schedule();  
    }
}

void clear_threads(){
    for(int i=0;i<thread_numbers;i++){
        if(threads[i] != NULL){
            free(threads[i]);
            threads[i] = NULL;
        }
    }
}

void handle_child(tid_t tid){
    struct thread_sibling *ts = threads[0]->childs;
    if(ts == NULL){
        return;
    }else{
        while(ts->self->status == dead){
            free(ts);
            threads[0]->childs = threads[0]->childs->next;
            ts = threads[0]->childs;
            if(ts == NULL) return;
        }
        while(ts->next != NULL){
            if(ts->next->self->status == dead){
                free(ts->next);
                ts->next = ts->next->next;
            }
            ts = ts->next;
        }
        
    }
}

void free_mem_table(struct thread *t){
    for(int i=0;i<256;i++){
        if(t->malloc_table[i] != NULL){
            free(t->malloc_table[i]);
            t->malloc_table[i] = NULL;
        }else{
            return;
        }
    }
}

void record_mem(void* addr){
    struct thread *t = get_current();
    for(int i=0;i<256;i++){
        if(t->malloc_table[i] == NULL){
            t->malloc_table[i] = addr;
            if(i<255) t->malloc_table[i+1] = NULL;
            break;
        }
    }
}

void delete_last_mem(){
    struct thread *t = get_current();
    if(t == NULL || t->malloc_table[0] == NULL) return;
    for(int i=1;i<256;i++){
        if(t->malloc_table[i] == NULL){
            t->malloc_table[i-1] = NULL;
            break;
        }
    }
}

void printf_thread(){
    for(int i=0;i<10;i++){
        if(threads[i] != NULL){
            uart_printf("tid: %d\n",threads[i]->tid);
            uart_printf("addr: 0x%x\n",threads[i]);
            uart_printf("status: %d\n",threads[i]->status);
            uart_printf("ptid: %d\n",threads[i]->ptid);
            uart_printf("stack: 0x%x ~ 0x%x\n",threads[i]->stack,threads[i]->stack+0x10000);
        }
    }
}