#include "mini_uart.h"
#define max_length 128

int main() {
    uart_init();
    uart_flush();
    char test[]={"Hello World!\n"};
    for(int i=0;i<sizeof(test);i++) uart_write(test[i]);
    return 0;
}
