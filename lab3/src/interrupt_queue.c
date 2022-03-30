#include "interrupt_queue.h"
#include "allocator.h"

struct interrupt_event *task_controller = NULL;

void push_queue(void (*callback)()){
    struct interrupt_event *tmp = simple_malloc(sizeof(struct interrupt_event));
    tmp->callback = callback;
    /*tmp->type = type;
    tmp->esr = esr;
    tmp->elr = elr;
    tmp->spsr = spsr;*/
    tmp->next = NULL;
    if(task_controller == NULL){
        task_controller = tmp;
    }else{
        struct interrupt_event *tmp2 = task_controller;
        while(tmp2->next != NULL) tmp2 = tmp2->next;
        tmp2->next = tmp;
    }
}

int task_empty(){
    return task_controller == NULL;
}

void exe_first_task(){
    if(task_controller == NULL) return;
    task_controller->callback();
    task_controller = task_controller->next;
}