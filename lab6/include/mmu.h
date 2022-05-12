#define pte_t unsigned long long
typedef struct{pte_t entries[512];} pagetable_t;
void disable_mmu();
void setup_mmu();
void set_ttbr0_el1(pagetable_t* pt);
pagetable_t* allocate_page();
void SetTaskStackPagetable(pagetable_t *pt, void* stack_addr);
void SetTaskCodePagetable(pagetable_t *pt, void* code_addr, unsigned long long size);

