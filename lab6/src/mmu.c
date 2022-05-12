#include "mmu.h"
#include "peripheral/mmu.h"
#include "uint.h"
#include "allocator.h"
#include "math.h"
#include "thread.h"

pagetable_t* allocate_page(){
    pagetable_t* pt = malloc(sizeof(pagetable_t));
    for(int i=0;i<512;i++) pt->entries[i] = NULL;
    return pt;
}

pte_t walk(pagetable_t *pt, uint64_t va, uint64_t end, uint64_t pa, uint64_t blocksize, bool writeable){
    if(blocksize < 1<<12){
        if(writeable) return (pa & 0xffffffff) | PTE_ATTR_STACK;
        else return (pa & 0xffffffff) | PTE_ATTR_CODE;
    }else{
        uint64_t start = va / blocksize;
        uint64_t num = upper_bound(end,blocksize) - va/blocksize;
        if(pt == NULL){
            pt = allocate_page();
            move_last_mem(0);
        }else{
            pt = (uint64_t)pt & ~(uint64_t)0x3;
        }
        for(int i=0;i<num;i++){
            uint64_t va2;
            if(i==0) va2 = va;
            else va2 = (va/blocksize + i) * blocksize;
            uint64_t gap = (va2/blocksize) * blocksize;
            pt->entries[start + i] = walk(pt->entries[start + i], va2 - gap,min(end,(va2/blocksize + 1) * blocksize) - gap,pa + va2 - va,blocksize >> (uint64_t)9,writeable);
        }
        return ((uint64_t)pt & (uint64_t)0xffffffff) | 0b11;
    }
}

void mappages(pagetable_t* pagetable, uint64_t va, uint64_t size, uint64_t pa){
    uint64_t end_addr = va + size;
    walk(pagetable, va, end_addr, pa, (uint64_t)1 << 39, va != 0);
}

void SetTaskStackPagetable(pagetable_t *pt, void* stack_addr){
    int stack_size = 0x4000;
    mappages(pt,0xffffffffb000,stack_size, (uint64_t)stack_addr & 0xfffffffc);
}

void SetTaskCodePagetable(pagetable_t *pt, void* code_addr, uint64_t size){
    mappages(pt,0x0, size, (uint64_t)code_addr & 0xfffffffc);
}
