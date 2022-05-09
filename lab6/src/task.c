#include "thread.h"
#include "uint.h"
#include "queue.h"
#include "task.h"
#include "scheduler.h"
#include "allocator.h"
#include "excep.h"

struct thread* ReadyList = NULL;
extern struct thread *threads[thread_numbers];

void UserScheduler(){
    if(ReadyList == NULL){
        core_timer_disable();
        exit();
    }else{
        struct thread *t = ReadyListPop();
        SwitchTo(t);
    }
}

void UserExit(){
    struct thread *t = get_current();
    t->status = dead;
    UserScheduler();
}

void* ReadyListPop(){
    void * tmp = ReadyList;
    ReadyList = ReadyList->next;
    return tmp;
}

void InitUserTaskScheduler(){
    thread_timer();
    struct thread *t = get_current();
    t->status = dead;
    UserScheduler();
}

void PushToReadyList(pid_t pid){
    struct thread* item = threads[pid];
    struct thread *tmp = ReadyList;
    if(tmp == NULL){
        ReadyList = item;
    }else{
        while(tmp->next != NULL) tmp = tmp->next;
        tmp->next = item;
    }
    item->next = NULL;
}

void RemoveItemFromReadyList(pid_t pid){
    struct thread *tmp = ReadyList;
    if(tmp != NULL){
        if(tmp->tid == pid){
            ReadyList = ReadyList->next;
        }else{
            while(tmp->next != NULL){
                if(tmp->next->tid == pid){
                    tmp->next = tmp->next->next;
                }
            }
        }
    }
}

int UserKill(pid_t pid){
    threads[pid]->status = dead;
    RemoveItemFromReadyList(pid);
}

int UserThread(void* func,void* arg){
    struct thread *t;
    t = malloc(sizeof(struct thread));
    unsigned char* kstack = malloc(0x10000);
    delete_last_mem();
    t->next = NULL;
    for(int i=0;i<32;i++) t->sig_handler[i] = NULL;
    t->sig_handler[9] = call_exit;
    t->sig_handler[10] = UserKill;
    t->signal = 0;
    t->childs = NULL;
    t->kstack = kstack;
    t->registers[0] = func;
    t->registers[1] = ( (uint64)(t->stack + 0x10000) & 0xfffffff0);
    t->registers[2] = arg;
    t->registers[3] = user_process;
    t->registers[10] = (unsigned long long)(kstack + 0x10000) & 0xfffffff0;
    t->registers[11] = from_el1_to_el0;
    t->registers[12] = t->registers[10];
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
    PushToReadyList(t->tid);
    return t->tid;
}

int set_fork(void* sp){
    byte *t = get_current();
    tid_t tid = UserThread(return_to_child,NULL);
    free(threads[tid]->kstack);
    byte *child = threads[tid];
    uint64 gap = (uint64)child - (uint64)t;
    for(int i=0;i<sizeof(struct thread);i++){
        child[i] = t[i];
    }
    unsigned char* kstack = malloc(0x10000);
    struct thread *tmp = t;
    struct trapframe *tf = (uint64)kstack + (uint64)sp - (uint64)tmp->kstack;
    threads[tid]->tid = tid;
    threads[tid]->ptid = tmp->tid;
    threads[tid]->registers[10] = tf-1;
    threads[tid]->registers[11] = return_to_child;
    threads[tid]->registers[12] = tf;
    threads[tid]->next = NULL;
    threads[tid]->kstack = kstack;
    for(int i=0;i<0x10000;i++) kstack[i] = tmp->kstack[i];
    tf->sp_el0 += gap;
    tf->spsr_el1 = 0;
    tf->x[0] = 0;
    tf->x[29] += gap;
    return tid;
}
