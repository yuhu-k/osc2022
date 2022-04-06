#include "mini_uart.h"
#include "string.h"
#include "reboot.h"
#include "uint.h"
#include "dtb.h"
#include "allocator.h"
#include "aux.h"


extern unsigned char __heap_start;
uint32 cpio_start,cpio_end;
char cmd_buffer[1024];
unsigned int cmd_index = 0;
unsigned int cmd_flag  = 0;


void shell_init(){
    uint32 *heap = (uint32*)(&__heap_start-8);
    *heap &= 0x00000000;
    uart_init();
    uart_printf("\n\n\nHello From RPI3\n");
    uart_init_buffer();
    uart_flush();
    core_timer_disable();
    init_allocator();
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

void reset_flag(){
    cmd_flag=0;
    *AUX_MU_IER = 1;
    core_timer_disable();
    irq_enable();
}

void uart_read_line(){
    char in;
    while(1){
        if(cmd_flag == 0){
            uart_printf("# ");
            cmd_flag = 1;
            cmd_index = 0;
            for(int i=0;i<1024;i++) cmd_buffer[i]=0;
        }
        while( uart_pop(&in) ){
            if(cmd_flag == 0){
                uart_printf("# ");
                cmd_flag = 1;
                cmd_index = 0;
                for(int i=0;i<1024;i++) cmd_buffer[i]=0;
            }
            if(in == 13){
                uart_printf("\n");
                cmd_buffer[cmd_index++] = '\0';
                cmd_flag = 0;
                check(cmd_buffer);
            }else if((in==8 || in==127)){
                if(cmd_index>0){
                    cmd_index--;
                    char t[1]={8};
                    cmd_buffer[cmd_index]='\0';
                    uart_write(8);
                    uart_write(' ');
                    uart_write(8);
                }
            }else if( in>=32 && in<=126 ){
                cmd_buffer[cmd_index++]=in;
                //uart_write(in);
                uart_push(in);
                *AUX_MU_IER |= 2;
            }
        }
    }
}

void* m[10];

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
        for(int i=0;i<128;i++) name[i] &= 0;
        int i=4;
        for(i=4;input[i]>=46 && input[i]<=122  && i<128 && input[i]!='\0'; i++){
            name[i-4]=input[i];
        }
        name[i]='\0';
        print_content(name, cpio_start);
    }else if(strncmp(input,"./",2)){
        /*char name[128];
        for(int i=0;i<128;i++) name[i] = 0;
        int i;
        for(i=2;input[i]>=46 && input[i]<=122  && i<128 && input[i]!='\0'; i++){
            name[i-2]=input[i];
        }
        execute(name,cpio_start);*/
        if(input[2] == '1')
            execute("program1.img\0",cpio_start);
        if(input[2] == '2')
            execute("program2.img\0",cpio_start);
        if(input[2] == '3')
            execute("program3.img\0",cpio_start);
    }else if(strncmp(input,"timer", 5)){
        if(input[5]!=' '){
            int clock_hz,now_time,interval;
            asm volatile("mrs %[input0], cntfrq_el0\n"
                         "mrs %[input1], cntp_tval_el0\n"
                         :[input0] "=r" (clock_hz),
                         [input1] "=r" (interval));
            uart_printf("%d\n", interval/clock_hz);
        }else{
            char name[128];
            for(int i=0;i<128;i++) name[i] = 0;
            int i;
            for(i=6;input[i]>=46 && input[i]<=122  && i<128 && input[i]!='\0'; i++){
                name[i-6]=input[i];
            }
            if(strncmp(name,"stop",4)){
                core_timer_disable();
            }
            else if(strncmp(name,"start",5)){
                core_timer_enable();
            }
        }
    }else if(strncmp(input,"sleep", 5)){
        char time[5];
        for(int i=0;i<5 && input[i+6]>=32 && input[i+6]<=127;i++) time[i] = input[i+6];
        sleep(atoi(time));
    }else if(strcmp(input,"mem")){
        if(input[4] != '-')
            show_status();
        else if(input[5] == 'a'){
            char size[128];
            for(int i=0;input[i+7]<='9' && input[i+7]>='0' && i<121;i++){
                size[i] = input[i+7];
                size[i+1]='\0';
            }
            int SIZE = atoi(size);
            for(int i=9;i>0;i--){
                m[i] = m[i-1];
            }
            m[0] = malloc(SIZE);
        }else if(input[5] == 'd'){
            free(m[0]);
            for(int i=0;i<9;i++){
                m[i] = m[i+1];
            }
            m[9] = NULL;
        }else if(input[5] == 's'){
            pool_status();
        }
    }else{
        uart_printf("command not found: %s\n",input);
    }
}