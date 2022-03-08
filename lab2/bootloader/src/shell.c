#include "mini_uart.h"
#include "string.h"
#include "reboot.h"
#include "loadimg.h"
#include "uint.h"

void shell_init(){
    uart_init();
    uart_flush();
    uart_printf("\n\nHello From RPI3\n");
}

void uart_read_line(char *fmt){
    uart_printf("# ");
    char in;
    int i=0;
    while(1){
        in=uart_read();
        if(in=='\n'){
            fmt[i++]=in;
            uart_printf("\n");
            break;
        }else if((in==8 || in==127)){
            if(i>0){
                i--;
                char t[1]={8};
                fmt[i]='\0';
                uart_printf("%s %s",t,t);
            }
            continue;
        }else if(in == 3){
            for(int j=0;j<i;j++) fmt[j] &= 0x00;
            uart_printf("^C\n# ");
            i=0;
            continue;
        }
        fmt[i++]=in;
        uart_write(in);
    }
    fmt[i]='\0';
}

void check(char *input){
    if(strcmp(input,"help")==1){
        uart_printf("help     : print the help menu\n");
        uart_printf("hello    : print Hello World!\n");
        uart_printf("reboot   : reboot the device\n");
        uart_printf("loadimg  : load image from user\n");
    }else if(strcmp(input,"hello")==1){
        uart_printf("Hello World!\n");
    }else if(strncmp(input,"reboot ",7)==1){
        uart_printf("Rebooting...\n");
        int a=0;
        for(int i=7;input[i]<'9' && input[i]>'0';i++){
            a *= 10;
            a += (input[i]-'0');
        }
        reset(0);
        while(1);
    }else if(strcmp(input,"loadimg")){
        uart_printf("Loading...\n");
        loadimg();
    }else{
        uart_printf("command not found: %s", input);
    }
}