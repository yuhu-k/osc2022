#include "mini_uart.h"
#include "uint.h"
#include "priority_queue.h"

int bp;


void arm_core_timer_intr_handler() {
    /*asm volatile("mrs x0, cntfrq_el0\n"
                 "msr cntp_tval_el0, x0\n");*/
    core_timer_enable();
    core_timer_handler();
}

void core_timer_handler(){
    struct node* node = delete_first_node();
    if(node->next == NULL){
        asm volatile("mrs x0, cntfrq_el0\n"
                     "ldr x1, =10\n"
                     "mul x0, x0, x1\n"
                     "msr cntp_tval_el0, x0\n");
        core_timer_disable();
    }else{
        asm volatile("msr cntp_tval_el0, %[output0]\n"
                     ::[output0] "r" (node->next->time_to_ring));
    }
    node->todo(node->arguments);
}

void add_timer(void (*callback_f)(void*),void *argu_for_call,int times){
    int clock_hz,now_time,interval;
    asm volatile("mrs %[input0], cntfrq_el0\n"
                 "mrs %[input2], cntp_tval_el0\n"
                 :[input0] "=r" (clock_hz),
                  [input2] "=r" (interval));
    int time_to_ring = add_node(callback_f, argu_for_call, clock_hz * times, interval);
    core_timer_enable();
    asm volatile("msr cntp_tval_el0, %[output0]\n"
                 ::[output0] "r" (time_to_ring));
}

void *wakeup(void *p){
    uart_printf("Timeout!!\n");
}

void sleep(int duration){
    add_timer(wakeup,NULL,duration);
    uart_printf("Timer is set...\n");
}


