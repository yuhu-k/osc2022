#include "uint.h"
#include "mini_uart.h"
#include "peripheral/irq.h"
#include "aux.h"


void enable_interrupt_controller() {
    uint32 *tmp = irq0_enable_1;
    *tmp |= (1 << 29);
}

void handle_irq() {
    uint32 *irq;
    irq = irq0_pending_1;

    while(*irq) {
        if (*irq & (1 << 29)) {
            *irq &= ~(1 << 29);
            while((*AUX_MU_IIR & 4) == 4) {
                uart_printf("UART Recv: ");
                uart_write(uart_read());
                uart_printf("\n");
            }
        }
    }

}