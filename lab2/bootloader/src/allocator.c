#include "uint.h"
#include "mini_uart.h"
extern unsigned char __heap_start;

void* simple_malloc(unsigned int size) {
    uint32 *temp=(uint32*)(&__heap_start-8);
    uint32 addr = ((uint32) &__heap_start) + *temp;
    *temp+=size;
    return (void*) addr;
}