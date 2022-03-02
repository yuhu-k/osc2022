#include "mini_uart.h"
#include "shell.h"
#include "mailbox.h"
#define max_length 128


#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

int main() {
    shell_init();
    uart_printf("Initial completed\n");
    get_board_revision();
    get_arm_memory();
    while (1) {
        uart_printf("# ");
        char input[max_length];
        uart_read_line(input);
        if(strcmp(input,"help")==1){
            uart_printf("help    : print the help menu\n");
            uart_printf("hello   : print Hello World!\n");
            uart_printf("reboot  : reboot the device\n");
        }else if(strcmp(input,"hello")==1){
            uart_printf("Hello World!\n");
        }else if(strcmp(input,"reboot")==1){
            uart_printf("Rebooting...\n");
            reset(100);
            while(1);
        }else{
            uart_printf("command not found: ");
            uart_printf(input);
        }
    }
}
