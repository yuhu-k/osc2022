#include "uint.h"
void* simple_malloc(unsigned int size);
void* malloc(size_t size);
void* getbestfit(int ind);
void show_status();
void init_allocator();
void free(void* address);
void pool_status();

struct FrameArray{
    int val, index;
    struct FrameArray* next;
    int allocatable;
};

struct mem_frag{
    void* start;
    struct mem_frag *next;
    int size;
};