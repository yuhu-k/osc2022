#include "mini_uart.h"
#define max_length 128

int main() {
    uart_write('H');
    uart_write('e');
    uart_write('l');
    uart_write('l');
    uart_write('o');
    uart_write(' ');
    uart_write('W');
    uart_write('o');
    uart_write('r');
    uart_write('l');
    uart_write('d');
    uart_write('!');
    uart_write('!');
    uart_write('\r');
    uart_write('\n');
    return 0;
}
