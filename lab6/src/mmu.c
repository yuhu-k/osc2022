#include "mmu.h"
#include "peripheral/mmu.h"
#include "uint.h"
#include "allocator.h"
#include "math.h"
#include "thread.h"
#include "scheduler.h"

bool vm_usable(uint64_t va, pagetable_t *p){
    pagetable_t *pt = ((uint64_t) p & 0xfffffffc) | 0xffff000000000000;
    uint64_t blocksize = 0x8000000000;
    for(int i=0;i<4;i++){
        pt = ((uint64_t)pt->entries[va/blocksize] & 0xfffffffc) | 0xffff000000000000;
        va %= blocksize;
        blocksize >>= 9;
        if(pt == 0xffff000000000000){
            return true;
        }
    }
    return false;
}

uint64_t mmu_decode(uint64_t va, pagetable_t *p){
    pagetable_t *pt = ((uint64_t) p & 0xfffffffc) | 0xffff000000000000;
    uint64_t blocksize = 0x8000000000;
    pt = ((uint64_t)pt->entries[va/blocksize] & 0xfffffffc) | 0xffff000000000000;
    va %= blocksize;
    blocksize >>= 9;
    pt = ((uint64_t)pt->entries[va/blocksize] & 0xfffffffc) | 0xffff000000000000;
    va %= blocksize;
    blocksize >>= 9;
    pt = ((uint64_t)pt->entries[va/blocksize] & 0xfffffffc) | 0xffff000000000000;
    va %= blocksize;
    blocksize >>= 9;
    pt = ((uint64_t)pt->entries[va/blocksize] & 0xfffffffc);
    va %= blocksize;
    uint64_t re = ((uint64_t)pt&0xfffffffff000) + va;
    return re;
}

pagetable_t* allocate_page(){
    pagetable_t* pt = malloc(sizeof(pagetable_t));
    for(int i=0;i<512;i++) pt->entries[i] = NULL;
    return pt;
}

pte_t walk(pagetable_t *pt, uint64_t va, uint64_t end, uint64_t pa, uint64_t blocksize, byte prot){
    if(blocksize < 1<<12){
        uint64_t label = PTE_ATTR_BASE;
        if(prot == 0) return (pa & 0xfffff000);
        if(prot & PROT_WRITE == 0) label |= RO_BIT | PD_ACCESS;
        else label |= PD_ACCESS;
        if(prot & PROT_EXEC == 0) label |= USR_EXE_NEVER_BIT;
        return (pa & 0xfffff000) | label;
    }else{
        uint64_t start = va / blocksize;
        uint64_t num = upper_bound(end,blocksize) - va/blocksize;
        if(pt == NULL){
            pt = allocate_page();
            move_last_mem(0);
        }else{
            pt = ((uint64_t)pt & ~(uint64_t)0x3) | 0xffff000000000000;
        }
        for(int i=0;i<num;i++){
            uint64_t va2;
            if(i==0) va2 = va;
            else va2 = (va/blocksize + i) * blocksize;
            uint64_t gap = (va2/blocksize) * blocksize;
            pt->entries[start + i] = walk(pt->entries[start + i], va2 - gap,min(end,(va2/blocksize + 1) * blocksize) - gap,pa + va2 - va,blocksize >> (uint64_t)9,prot);
        }
        return ((uint64_t)pt & (uint64_t)0xffffffff) | 0b11;
    }
}

void mappages(pagetable_t* pagetable, uint64_t va, uint64_t size, uint64_t pa, byte prot){
    uint64_t end_addr = va + size;
    walk(pagetable, va, end_addr, pa, (uint64_t)1 << 39, prot);
}

void SetTaskStackPagetable(pagetable_t *pt, void* stack_addr){
    int stack_size = 0x4000;
    mappages(pt,0xffffffffb000,stack_size, (uint64_t)stack_addr & 0xfffffffc,PROT_READ | PROT_WRITE);
}

void SetTaskCodePagetable(pagetable_t *pt, void* code_addr, uint64_t size){
    mappages(pt,0x0, size, (uint64_t)code_addr & 0xfffffffc, PROT_READ | PROT_EXEC);
}

void SetPeripherialPagetable(pagetable_t *pt){
    mappages(pt,0x3c000000, 0x4000000, 0x3c000000, PROT_READ | PROT_WRITE);
}

void* mmap_set(void* addr, size_t len, int prot, int flags){
    uint64_t align_addr = (uint64_t)addr & ~(uint64_t)0xfff;
    thread_t *t = get_current();
    while(!vm_usable(align_addr,t->page_table)){
        align_addr += 0x1000;
        if(align_addr >= 0xffffffffb000) return NULL;
    }
    for(int i=0;i<len>>12;i++) mappages(t->page_table,align_addr + i * 0x1000,0x1000,(uint64_t)prot<<12,0);

    return align_addr;
}

bool mmap_check(uint64_t FAR){
    uint64_t align_addr = (uint64_t)FAR & ~(uint64_t)0xfff;
    thread_t *t = get_current();
    uint64_t prot = mmu_decode(align_addr,t->page_table)>>12;
    if(prot != 0){
        void *pa = malloc(0x1000);
        mappages(t->page_table,align_addr,0x1000,pa,prot);
        return true;
    }
    return false;
}