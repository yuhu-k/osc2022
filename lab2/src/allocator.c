#include "uint.h"
#include "mini_uart.h"
extern unsigned char __heap_start;

void* simple_malloc(unsigned int size) {
    int used_space=0;
    byte *temp=(byte*)&__heap_start;
    used_space += *(temp);
    used_space <<= 8;
    used_space += *(temp+1);
    uint32 addr = ((uint32) &__heap_start) + used_space;
    used_space+=size;
    *(temp+1) = used_space%256;
    used_space >>= 8;
    *temp = used_space;
    return (void*) addr;
}