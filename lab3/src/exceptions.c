#include "mini_uart.h"
#include "irq.h"
#define uart_puts uart_printf
/**
 * common exception handler
 */
void exception_entry(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr)
{
    // print out interruption type
    switch(type%4) {
        case 0: uart_puts("Synchronous"); break;
        case 1: handle_irq(); uart_puts("IRQ"); break;
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
    // dump registers
    uart_puts(":\n ESR_EL1 ");
    uart_printf("0x%x",esr>>32);
    uart_printf("0x%x",esr);
    uart_puts("\n ELR_EL1 ");
    uart_printf("0x%x",elr>>32);
    uart_printf("0x%x",elr);
    uart_puts("\n SPSR_EL1 ");
    uart_printf("0x%x",spsr>>32);
    uart_printf("0x%x",spsr);
    uart_puts("\n");
    // no return from exception for now
    //while(1);
    err_han();
}
