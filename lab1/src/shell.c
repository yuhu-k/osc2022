#include "mini_uart.h"
#include "string.h"
void shell_init(){
    uart_init();
    uart_flush();
    uart_printf("\n\nHello From RPI3\n");
}

void uart_read_line(char *input){
    char in;
    int i=0;
    while(1){
        in=uart_read();
        if(in=='\n'){
            input[i++]=in;
            uart_printf("\n");
            break;
        }else if((in==8 || in==127) && i>0){
            i--;
            input[i]='\0';
            uart_printf("\r# ");
            uart_printf(input);
            continue;
        }
        input[i++]=in;
        uart_write(in);
    }
    input[i]='\0';
}

