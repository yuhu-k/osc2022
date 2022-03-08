#include "mini_uart.h"
#include "string.h"
#include "reboot.h"
#include "cpio.h"
#include "allocator.h"
#include "uint.h"
extern byte __heap_start;

void shell_init(){
    byte *addr = (char *) &__heap_start;
    *addr &= 0x00;
    *(addr+1) &= 0x00;
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
            fmt[i++]='\0';
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
        }else if( in>=32 && in<=126 ){
            fmt[i++]=in;
            uart_write(in);
        }
    }
    fmt[i]='\0';
}

void check(char *input){
    if(input[0] == '\0' || input[0] == '\n') return;
    if(strcmp(input,"help")==1){
        uart_printf("help    : print the help menu\n");
        uart_printf("hello   : print Hello World!\n");
        uart_printf("reboot  : reboot the device\n");
    }else if(strcmp(input,"hello")==1){
        uart_printf("Hello World!\n");
    }else if(strncmp(input,"reboot ",7)==1){
        uart_printf("Rebooting...\n");
        int a=0;
        for(int i=7;input[i]<='9' && input[i]>='0';i++){
            a *= 10;
            a += (input[i]-'0');
        }
        reset(a<50? 50:a);
        while(1);
    }else if(strcmp(input,"ls")){
        list(0x20000000);
    }else if(strncmp(input,"cat ", 4)){
        char name[128];
        int i=4;
        for(i=4;input[i]>=46 && input[i]<=122  && i<128 && input[i]!='\0'; i++){
            name[i-4]=input[i];
        }
        name[i]='\0';
        print_content(name, 0x20000000);
    }else{
        uart_printf("command not found: %s\n",input);
    }
}