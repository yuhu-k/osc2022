#include "uint.h"
void* simple_malloc(unsigned int size);
void* malloc(unsigned int size);
void* getbestfit(int ind);
void show_status();
void init_allocator();
void free(void* address);

struct FrameArray{
    int val, index;
    struct FrameArray* next;
    int allocatable;
};