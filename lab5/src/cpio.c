#include "uint.h"
#include "mini_uart.h"
#include "string.h"
#include "excep.h"
#include "thread.h"
#include "queue.h"
#include "scheduler.h"
#include "task.h"
extern unsigned int cpio_start;

typedef struct cpio_newc_header {  //cpio new ascii struct
		   char	   c_magic[6];
		   char	   c_ino[8];
		   char	   c_mode[8];
		   char	   c_uid[8];
		   char	   c_gid[8];
		   char	   c_nlink[8];
		   char	   c_mtime[8];
		   char	   c_filesize[8];
		   char	   c_devmajor[8];
		   char	   c_devminor[8];
		   char	   c_rdevmajor[8];
		   char	   c_rdevminor[8];
		   char	   c_namesize[8];
		   char	   c_check[8];
}CPIO_H ;


void list(uint32 addr){ // proceed ls
    CPIO_H *cpio=(CPIO_H*) addr;
    while(strncmp(cpio->c_magic,"070701\0",6)==1){ //c_magic is always "070701"
        int namesize=a16ntoi(cpio->c_namesize, 8);
        char *name=(char*)(cpio+1);
        char temp[128];
        for(int i=0;i<namesize;i++){
            temp[i]=name[i];
        }
        temp[namesize]='\0';
        if(strncmp(temp, "TRAILER!!!", 10)) return;
        int filesize=a16ntoi(cpio->c_filesize, 8);
        uart_printf("%s\n",temp);

        int sum=110+namesize+filesize;
        if(sum%4 != 0){
            sum/=4;
            sum*=4;
            sum+=4;
        }
        addr+=sum;  // padding is included in namesize and filesize

        cpio=(CPIO_H*) addr;
    }
}

uint32 find_file_addr(char *file_name, uint32 addr){
    CPIO_H *cpio=(CPIO_H*) addr;
    while(strncmp(cpio->c_magic,"070701\0",6)==1){
        int namesize=a16ntoi(cpio->c_namesize, 8);
        char *name=(char*)(cpio+1);
        char temp[128];
        for(int i=0;i<namesize;i++){
            temp[i]=name[i];
        }
        temp[namesize]='\0';
        if(strncmp(temp, "TRAILER!!!", 10)) break;
        int filesize=a16ntoi(cpio->c_filesize, 8);
        if(strcmp(file_name, temp)){
            return addr;
        }
        int sum=110+namesize+filesize;
        if(sum%4 != 0){
            sum/=4;
            sum*=4;
            sum+=4;
        }
        addr+=sum;  // padding is included in namesize and filesize
        cpio=(CPIO_H*) addr;
    }
    return NULL;
}

void* getContent(uint32 addr,int *length){
    CPIO_H *cpio=(CPIO_H*) addr;
    int namesize=a16ntoi(cpio->c_namesize, 8);
    int filesize=a16ntoi(cpio->c_filesize, 8);
    addr += (110+namesize);
    *length = filesize;
    return addr;
}

void print_content(char *file, uint32 addr){
    uint32 f_addr=find_file_addr(file,addr);
    if(f_addr != NULL){
        int length;
        char *content_addr = getContent(f_addr, &length);
        while(length--){
            if(*content_addr == '\n') uart_write('\r');
            uart_write(*content_addr++);
        }
        return;
    }else{
        uart_printf("Not found file \"%s\"\n",file);
        return;
    }
}

void execute(char *file,char *const argv[]){
    uint32 f_addr=find_file_addr(file,cpio_start);
    if(f_addr != NULL){
        int length;
        uint32 address = getContent(f_addr, &length);
        char *content_addr = address;
        if(address%4 != 0){
            address /= 4;
            address *= 4;
            address += 4;
        }
        void * start = malloc(length + 0x20000);
        byte *ucode = (uint64)start + 0x10000;
        byte *file = address;
        for(int i=0;i<length;i++){
            ucode[i] = file[i];
        }
        int tid = UserThread(ucode,NULL);
        remove_from_queue(tid);
        PushToReadyList(tid);
        move_last_mem(0);
        tid = Thread(InitUserTaskScheduler);
        return;
    }else{
        uart_printf("Not found file \"%s\"\n",file);
        return;
    }
}

void exec(char *file,char *const argv[]){
    uint32 f_addr=find_file_addr(file,cpio_start);
    if(f_addr != NULL){
        int length;
        uint32 address = getContent(f_addr, &length);
        char *content_addr = address;
        if(address%4 != 0){
            address /= 4;
            address *= 4;
            address += 4;
        }
        void * start = malloc(length + 0x20000);
        byte *ucode = (uint64)start + 0x10000;
        byte *file = address;
        for(int i=0;i<length;i++){
            ucode[i] = file[i];
        }
        move_last_mem(0);
        struct thread *t = get_current();
        from_el1_to_el0(ucode, t->stack + 65535);
        return;
    }else{
        uart_printf("Not found file \"%s\"\n",file);
        return;
    }
}