#include "mini_uart.h"
#include "irq.h"
#include "peripheral/irq.h"
#include "timer.h"
#include "interrupt_queue.h"
#include "reboot.h"
#include "thread.h"
#include "cpio.h"
#include "mailbox.h"
#include "queue.h"
#define uart_puts uart_printf
/**
 * common exception handler
 */




struct trapframe {
    uint64 x[31]; // general register from x0 ~ x30
    uint64 spsr_el1;
    uint64 elr_el1;
};

void exception_entry(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, void* sp_addr)
{
    // print out interruption type
    switch(type%4) {
        case 0: 
        {
            if(esr>>26 == 0b010101){
                asm volatile("msr DAIFClr, 0xf\n");
                struct trapframe *tf = sp_addr;
                switch(tf->x[8]){
                    case 0:
                        tf->x[0] = getpid();
                        return;
                        break;
                    case 1:{
                        char *buf = tf->x[0], temp[256];
                        int rn = 0;
                        while(1){
                            temp[0] = 0;
                            uart_pop(temp);
                            for(int i=0;i<256;i++){
                                if(temp[i] == 0) break;
                                buf[rn++] = temp[i];
                                if(rn >= tf->x[1]){
                                    tf->x[0] = tf->x[1];
                                    return;
                                }
                            }
                        }
                        break;
                    }
                    case 2:{
                        char *buf = tf->x[0];
                        for(int i=0;i<tf->x[1];i++){
                            uart_write(buf[i]);
                        }
                        tf->x[0] = tf->x[1];
                        return;
                        break;
                    }
                    case 3:
                        execute(tf->x[0],tf->x[1]);
                        return;
                        break;
                    case 4:
                        tf->x[0] = set_fork(tf,sp_addr);
                        return;
                        break;
                    case 5:
                        exit();
                        break;
                    case 6:
                        tf->x[0] = mailbox_call(tf->x[1],tf->x[0]);
                        return;
                        break;
                    case 7:
                        kill(tf->x[0]);
                        return;
                        break;
                    default:
                        break;
                }
            }
            uart_puts("Synchronous"); 
            break;
        }
        case 1: 
            if(handle_irq()){
                asm volatile("msr DAIFClr, 0xf\n");
                exe_first_task(); 
                return; 
            }
            uart_puts("IRQ"); 
            break;
        case 2: uart_puts("FIQ"); break;
        case 3: uart_puts("SError"); break;
    }
    uart_puts(": ");
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch(esr>>26) {
        case 0b000000: uart_puts("Unknown"); break;
        case 0b000001: uart_puts("Trapped WFI/WFE"); break;
        case 0b001110: uart_puts("Illegal execution"); break;
        case 0b010101: uart_puts("System call"); break;
        case 0b100000: uart_puts("Instruction abort, lower EL"); break;
        case 0b100001: uart_puts("Instruction abort, same EL"); break;
        case 0b100010: uart_puts("Instruction alignment fault"); break;
        case 0b100100: uart_puts("Data abort, lower EL"); break;
        case 0b100101: uart_puts("Data abort, same EL"); break;
        case 0b100110: uart_puts("Stack alignment fault"); break;
        case 0b101100: uart_puts("Floating point"); break;
        default: uart_puts("Unknown"); break;
    }
    // decode data abort cause
    if(esr>>26==0b100100 || esr>>26==0b100101) {
        uart_puts(", ");
        switch((esr>>2)&0x3) {
            case 0: uart_puts("Address size fault"); break;
            case 1: uart_puts("Translation fault"); break;
            case 2: uart_puts("Access flag fault"); break;
            case 3: uart_puts("Permission fault"); break;
        }
        switch(esr&0x3) {
            case 0: uart_puts(" at level 0"); break;
            case 1: uart_puts(" at level 1"); break;
            case 2: uart_puts(" at level 2"); break;
            case 3: uart_puts(" at level 3"); break;
        }
    }

    int ec = (esr >> 26) & 0b111111;
    int iss = esr & 0x1FFFFFF;
    int cntfrq_el0,cntpct_el0;
    // system call
    if (ec == 0b010101) {  
        switch (iss) {
            case 1:
                uart_printf("Exception return address 0x%x\n", elr);
                uart_printf("Exception class (EC) 0x%x\n", ec);
                uart_printf("Instruction specific syndrome (ISS) 0x%x\n", iss);
                break;
            case 2:
                core_timer_enable();
                break;
            case 3:
                core_timer_disable();
                break;
            case 4:
                asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq_el0)); // get current counter frequency
                asm volatile ("mrs %0, cntpct_el0" : "=r" (cntpct_el0)); // read current counter
                break;
        }
    }
    // dump registers
    uart_puts(":\n ESR_EL1 ");
    uart_printf("0x%x ",esr>>32);
    uart_printf("0x%x",esr);
    uart_puts("\n ELR_EL1 ");
    uart_printf("0x%x ",elr>>32);
    uart_printf("0x%x",elr);
    uart_puts("\n SPSR_EL1 ");
    uart_printf("0x%x ",spsr>>32);
    uart_printf("0x%x",spsr);
    uart_puts("\n");
    if (type%4 == 0) reset(50);
}
