#include "mini_uart.h"
#include "shell.h"
#include "mailbox.h"
#define max_length 128


int main() {
    shell_init();
    uart_printf("Initial completed\n");
    get_board_revision();
    get_arm_memory();
    while (1) {
        char input[max_length];
        for(int i=0;i<max_length;i++) input[i]&=0;
        uart_read_line(input);
        check(input);
    }
}
