#include "mini_uart.h"
#include "shell.h"
#include "mailbox.h"
#include "irq.h"
#include "timer.h"
#define max_length 128

//extern struct commands COM;

int gettime(){
    int time,freq;
    asm volatile("mrs  %[input0], cntpct_el0\n"
                 "mrs  %[input1], cntfrq_el0\n"
                 :[input0] "=r" (time), [input1] "=r" (freq));
    float temp=(float)time/freq;
    int start_time= temp*1000;
    return start_time;
}

int main() {
    shell_init();
    uart_printf("Initial completed\n");
    get_board_revision();
    get_arm_memory();

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

    core_timer_enable();
    //timer_init();   //local timer
    uart_printf("# ");
    
    while (1) {
    }
}
