#include "mini_uart.h"
#include "uint.h"
uint32 arm_core_timer_jiffies=0;


void arm_core_timer_intr_handler() {
    asm volatile("bl   core_timer_handler\n");
    uart_printf("Core timer interrupt, %d\n", ++arm_core_timer_jiffies);
}