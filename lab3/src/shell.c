#include "mini_uart.h"
#include "string.h"
#include "reboot.h"
#include "uint.h"
#include "dtb.h"

extern unsigned char __heap_start;
uint32 cpio_start,cpio_end;

void shell_init(){
    uart_init();
    uart_flush();
    uart_printf("\n\nHello From RPI3\n");
    uint32 *heap = (uint32*)(&__heap_start-8);
    *heap &= 0x00000000;
    uint32 *ramf_start,*ramf_end;
    ramf_start=find_property_value("/chosen\0","linux,initrd-start\0");  //get ramf start addr from dtb
    ramf_end=find_property_value("/chosen\0","linux,initrd-end\0"); //get ramf end addr from dtb
    if(ramf_start != 0){
        uart_printf("Ramf start: 0x%x\n",letobe(*ramf_start));
        cpio_start=letobe(*ramf_start);
    }if(ramf_end != 0){
        uart_printf("Ramf end: 0x%x\n",letobe(*ramf_end));
        cpio_end=letobe(*ramf_end);
    }
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
    }else if(strncmp(input,"reboot",6)==1){
        uart_printf("Rebooting...\n");
        if(input[6] != ' '){
            reset(50);
            while(1);
        }
        int a=0;
        for(int i=7;input[i]<='9' && input[i]>='0';i++){
            a *= 10;
            a += (input[i]-'0');
        }
        reset(a<50? 50:a);
        while(1);
    }else if(strcmp(input,"ls")){
        list(cpio_start);
    }else if(strncmp(input,"cat ", 4)){
        char name[128];
        int i=4;
        for(i=4;input[i]>=46 && input[i]<=122  && i<128 && input[i]!='\0'; i++){
            name[i-4]=input[i];
        }
        name[i]='\0';
        print_content(name, cpio_start);
    }else{
        uart_printf("command not found: %s\n",input);
    }
}