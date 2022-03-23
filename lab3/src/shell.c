#include "mini_uart.h"
#include "string.h"
#include "reboot.h"
#include "uint.h"
#include "dtb.h"
#include "allocator.h"

#define NULL (void*)0

extern unsigned char __heap_start;
uint32 cpio_start,cpio_end;

/*struct commands{
    char *command;
    unsigned int length;
    struct commands *last,*next;
}*COM;

struct commands* init_commands(){
    struct commands *c;
    c=simple_malloc(sizeof(struct commands));
    c->length=0;
    c->last=NULL;
    c->next=NULL;
    return c;
}*/

void shell_init(){
    uart_init();
    uart_flush();
    uart_printf("\n\nHello From RPI3\n");
    uint32 *heap = (uint32*)(&__heap_start-8);
    *heap &= 0x00000000;
    /*uint32 *ramf_start,*ramf_end;
    ramf_start=find_property_value("/chosen\0","linux,initrd-start\0");  //get ramf start addr from dtb
    ramf_end=find_property_value("/chosen\0","linux,initrd-end\0"); //get ramf end addr from dtb
    if(ramf_start != 0){
        uart_printf("Ramf start: 0x%x\n",letobe(*ramf_start));
        cpio_start=letobe(*ramf_start);
    }if(ramf_end != 0){
        uart_printf("Ramf end: 0x%x\n",letobe(*ramf_end));
        cpio_end=letobe(*ramf_end);
    }*/
    //COM=init_commands();
}

void uart_read_line(char *fmt){
    char *store_fmt;
    store_fmt=fmt;
    //COM->command=fmt;

    //struct commands* temp=COM;
    uart_printf("# ");
    //char t2[128];
    //for(int i=0;i<128;i++) t2[i]=' ';
    char in;
    int i=0;
    while(1){
        in=uart_read();
        if(in=='\n'){
            uart_printf("\n");
            break;
        }/*else if( in == 'A' && store_fmt[i-1] == '['){
            if(temp->last==NULL) continue;
            uart_printf("\r%s\r# ",t2);
            temp=temp->last;
            store_fmt=temp->command;
            i=temp->length;
            //uart_printf("%s",store_fmt);
        }else if( in == 'B' && store_fmt[i-1] == '['){
            if(temp->next==NULL) continue;
            uart_printf("\r%s\r# ",t2);
            temp=temp->next;
            store_fmt=temp->command;
            i=temp->length;
            //uart_printf("%s",store_fmt);
        }*/else if((in==8 || in==127)){
            if(i>0){
                i--;
                char t[1]={8};
                store_fmt[i]='\0';
                uart_printf("%s %s",t,t);
            }
            continue;
        }else if(in == 3){
            for(int j=0;j<i;j++) store_fmt[j] &= 0x00;
            uart_printf("^C\n# ");
            i=0;
            continue;
        }else if( in>=32 && in<=126 ){
            store_fmt[i++]=in;
            uart_write(in);
        }
    }
    store_fmt[i]='\0';

    //strcpy(store_fmt,fmt,i+1);
    /*COM->next=init_commands();
    COM->next->last=COM;
    COM=COM->next;*/
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
    }else if(strcmp(input,"echo")){
        __asm__ volatile("bl      from_el1_to_el0\n"
                         "ldr     x1, =0x40000\n"
                         "mov     sp, x1\n");
        uart_printf("goto el0 successfully\n");
    }else{
        uart_printf("command not found: %s\n",input);
    }
}