#include "mini_uart.h"
#include "uint.h"
#include "priority_queue.h"
uint32 counter=0;
//msr DAIFSet, 0xf


void arm_core_timer_intr_handler() {
    asm volatile("mov  x22, lr\n"
                 "mrs x0, cntfrq_el0\n"
                 "msr cntp_tval_el0, x0\n"
                 "mov  lr, x22\n");
    uart_printf("Core timer interrupt, %d\n", ++counter);
    //core_timer_handler();
}

void core_timer_handler(){
    struct node* node = delete_first_node();
    if(node->next == NULL){
        asm volatile("ldr x0, #100000\n"
                     "msr cntp_tval_el0, x0\n");
        core_timer_disable();
    }else{
        int clock_hz,interval;
        asm volatile("mrs %[input0], cntfrq_el0\n"
                     :[input0] "=r" (clock_hz));
        interval = clock_hz*node->time_to_ring;
        asm volatile("msr cntp_tval_el0, %[output0]\n"
                     ::[output0] "r" (interval));
    }
    node->todo(node->arguments);
}

void add_timer(void (*callback_f)(void),void *argu_for_call,int times){
    int clock_hz,now_time,interval;
    asm volatile("mrs %[input0], cntfrq_el0\n"
                 "mrs %[input1], cntpct_el0\n"
                 "mrs %[input2], cntp_tval_el0\n"
                 :[input0] "=r" (clock_hz),
                  [input1] "=r" (now_time),
                  [input2] "=r" (interval));
    //uart_printf("%d %d %d\n",clock_hz,now_time,interval);
    int time_to_ring = add_node(callback_f,argu_for_call, times, ((float)interval-now_time)/clock_hz);
    core_timer_enable();

    
    interval = clock_hz*time_to_ring;
    /*asm volatile("msr cntp_tval_el0, %[output0]\n"
                 ::[output0] "r" (interval));*/
    asm volatile("mrs x0, cntfrq_el0\n"
                 "ldr x1, =5\n"
                 "mul x0, x0, x1\n"
                 "msr cntp_tval_el0, x0\n");
}

void *wakeup(void *p){
    int *pos = (int*)p;
    void (*goback)(void) = (void*) *pos;
    goback();
}

void sleep(float duration){
    uint32 back_pos;
    asm volatile("mov %[input0], lr\n"
                 : [input0] "=r" (back_pos));
    add_timer(wakeup,&back_pos,duration);
    while(1);
}


