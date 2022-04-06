#include "uint.h"
#include "mini_uart.h"
#include "allocator.h"
#include "math.h"
#define base_addr 0x10000000
#define end_addr 0x20000000
#define page_size 4096    //4KB
extern unsigned char __heap_start;


uint32 frame_num;

struct FrameArray** frame_list;
struct FrameArray* frame_array;



void* simple_malloc(unsigned int size) {
    uint32 *temp=(uint32*)(&__heap_start-8);
    uint32 addr = ((uint32) &__heap_start) + *temp;
    *temp+=size;
    return (void*) addr;
}

void init_allocator(){
    frame_num = (end_addr - base_addr)/page_size;
    frame_array = simple_malloc(frame_num * sizeof(struct FrameArray));
    frame_array[0].val = log2(frame_num);
    frame_array[0].index = 0;
    frame_array[0].next =NULL;
    frame_array[0].allocatable = 1;
    for(int i=1;i<frame_num;i++){
        frame_array[i].val = -1;
        frame_array[i].index = i;
        frame_array[i].allocatable = 1;
        frame_array[i].next =NULL;
    }
    frame_list = simple_malloc((log2(frame_num)+1) * sizeof(struct FrameArray*));
    frame_list[0] = &frame_array[0];
    for(int i=1;i<(log2(frame_num)+1);i++){
        frame_list[i] = NULL;
    }
}

void* malloc(unsigned int size){
    int mag, ind;
    for(mag = 1, ind=0; size > mag*page_size; mag *= 2,ind++);
    return getbestfit(ind);
}

void* getbestfit(int ind){
    int i;
    for(i=0;i+ind<=log2(frame_num);i++) 
        if(frame_list[(int)log2(frame_num)-ind-i] != NULL)
            break;
    while(i != 0){
        struct FrameArray* tmp=frame_list[(int)log2(frame_num)-ind-i];
        frame_list[(int)log2(frame_num)-ind-i] = (frame_list[(int)log2(frame_num)-ind-i])->next;
        unsigned int n= exp2(tmp->val-1);
        struct FrameArray* tmp2=&frame_array[tmp->index + n];
        tmp->next = tmp2;
        tmp->val -= 1;
        tmp2->next = NULL;
        tmp2->val = tmp->val;
        i--;
        frame_list[(int)log2(frame_num)-ind-i] = tmp;
    }


    struct FrameArray* tmp = frame_list[(int)log2(frame_num)-ind];
    frame_list[(int)log2(frame_num)-ind] = (frame_list[(int)log2(frame_num)-ind])->next;
    tmp->allocatable = 0;
    tmp->next = NULL;

    uart_printf("0x%x\n",base_addr + page_size*tmp->index);
    return (void*)(base_addr + page_size*tmp->index);
}

void show_status(){
    int i=0;
    uart_printf("0x%x: ",base_addr);
    while(i < frame_num){
        if(i != 0) uart_write('|');
        int length = exp2(frame_array[i].val);
        char a = frame_array[i].allocatable + '0';
        for(int j=0;j<length;j++) uart_write(a);
        i += length;
    }
    uart_printf(" 0x%x\n",end_addr);
}

void free(void* var){
    struct FrameArray* tmp = var;
    int index = ((int)var - base_addr) / page_size;
    if( frame_array[index].allocatable == 1 ) return;
    else{
        frame_array[index].allocatable = 1;
        merge(index);
    }
}

void merge(int index){
    uart_printf("%d\n",index);
    int length = frame_array[index].val;
    int front,back;
    if(index == 0){
        front = 0;
        back = index + (int)exp2(length);
    }else{
        if(log2(index) > length){
            front = index;
            back = index + (int)exp2(length);
        }else{
            front = index - (int)exp2(length);
            back = index;
        }
    }
    if(frame_array[front].val == frame_array[back].val && frame_array[front].allocatable && frame_array[back].allocatable){
        struct FrameArray *tmp = frame_list[(int)log2(frame_num) - length];
        if(tmp->index == index + (int)exp2(length)){
            frame_list[(int)log2(frame_num) - length] = frame_list[(int)log2(frame_num) - length]->next;
        }
        else {
            int the_removed = index == front? back:front;
            while(tmp->next != NULL){
                if(tmp->next->index == the_removed){
                    tmp->next = tmp->next->next;
                    break;
                }
            }
        }
        frame_array[front].val += 1;
        frame_array[back].val = -1;
        merge(front);
    }else{
        struct FrameArray *tmp = frame_list[(int)log2(frame_num) - length];
        if(tmp == NULL || tmp->index > index) 
            frame_list[(int)log2(frame_num) - length] = &(frame_array[index]);
        else{
            while(tmp != NULL && tmp->next != NULL){
                if(tmp->next->index > index){
                    frame_array[index].next = tmp->next;
                    tmp->next = &(frame_array[index]);
                    break;
                }
            }
            if(tmp->next == NULL){
                tmp->next = &(frame_array[index]);
                frame_array[index].next =NULL;
            }
        }
    }
}