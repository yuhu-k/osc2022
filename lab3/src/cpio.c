#include "uint.h"
#include "mini_uart.h"
#include "string.h"

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

int a16toi(char *num, int length){  // transform hex string to int
    int namesize=0;
    for(int i=0;i<length;i++){
        namesize <<= 4;
        if(num[i]>='0' && num[i]<='9'){
            namesize += (num[i]-'0');
        }else if(num[i]>='A' && num[i]<='F'){
            namesize += (num[i]-'A'+10);
        }
    }
    return namesize;
}

void list(uint32 addr){ // proceed ls
    CPIO_H *cpio=(CPIO_H*) addr;
    while(strncmp(cpio->c_magic,"070701\0",6)==1){ //c_magic is always "070701"
        int namesize=a16toi(cpio->c_namesize, 8);

        char *name=(char*)(cpio+1);
        char temp[128];
        for(int i=0;i<namesize;i++){
            temp[i]=name[i];
        }
        temp[namesize]='\0';
        int filesize=a16toi(cpio->c_filesize, 8);
        uart_printf("%s\n",temp);

        addr+=(110+namesize+filesize);  // padding is included in namesize and filesize
        cpio=(CPIO_H*) addr;
    }
}

void print_content(char *file, uint32 addr){
    CPIO_H *cpio=(CPIO_H*) addr;
    while(strncmp(cpio->c_magic,"070701\0",6)==1){
        int namesize=a16toi(cpio->c_namesize, 8);
        char *name=(char*)(cpio+1);
        char temp[128];
        for(int i=0;i<namesize;i++){
            temp[i]=name[i];
        }
        temp[namesize]='\0';
        int filesize=a16toi(cpio->c_filesize, 8);
        if(strcmp(file, temp)){
            addr += (110+namesize);
            char *w=(char*)addr;
            while(filesize--){
                if(*w=='\n') uart_write('\r');
                else if(*w=='\0') return;
                uart_write(*w++);
            }
            return;
        }
        addr+=(110+namesize+filesize);
        cpio=(CPIO_H*) addr;
    }
    uart_printf("Not found file \"%s\"\n",file);
}