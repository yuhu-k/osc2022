.globl irq_init_vectors
irq_init_vectors:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret
.global irq_enable
irq_enable:
    msr daifclr, #2
    ret

.global irq_disable
irq_disable:
    msr daifset, #2
    ret