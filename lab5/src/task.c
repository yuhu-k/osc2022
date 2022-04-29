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
    threads[pid] = NULL;
    RemoveItemFromReadyList(pid);
}

int UserThread(void* func,void* arg){
    struct thread *t;
    t = malloc(sizeof(struct thread));
    delete_last_mem();
    t->next = NULL;
    for(int i=0;i<32;i++) t->sig_handler[i] = NULL;
    t->sig_handler[9] = UserExit;
    t->sig_handler[10] = UserKill;
    t->signal = 0;
    t->childs = NULL;
    t->registers[0] = func;
    t->registers[1] = ( (uint64)(t->stack + 0x10000) & 0xfffffff0);
    t->registers[2] = arg;
    t->registers[3] = user_process;
    t->registers[10] = ( (uint64)(t->stack + 0x10000) & 0xfffffff0);
    t->registers[11] = from_el1_to_el0;
    t->registers[12] = ( (uint64)(t->stack + 0x10000) & 0xfffffff0);
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
