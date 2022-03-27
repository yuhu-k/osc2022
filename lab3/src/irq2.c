#include "uint.h"
#include "mini_uart.h"
#include "peripheral/irq.h"
#include "aux.h"
#include "timer.h"


void enable_interrupt_controller() {
    uint32 *tmp = irq0_enable_1;
    *tmp |= (1 << 29) | 2 ;
}

byte handle_irq() {
    uint32 *irq;
    irq = irq0_pending_1;
    byte con=0;

    if (*irq == 0) {
        arm_core_timer_intr_handler();
        return 1;
    }
    if (*irq & (1 << 29)) {
        *irq &= ~(1 << 29);
        uart_write(uart_read());
        uart_printf("\n");
        con=1;
    }
    if (*irq & 2) {
        *irq &= ~2;
        handle_timer_1();
        con=1;
    }
    return con;

}
