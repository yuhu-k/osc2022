
.section ".text.boot"

.global _start

_start:
1:
    // set stack pointer
    ldr     x1, =_start
    mov     sp, x1

    ldr     x0, =_lr_addr_
    str     lr, [x0]

2:  bl      main
    
    ldr     x0, =_lr_addr_
    ldr     lr, [x0]
    ret     lr
