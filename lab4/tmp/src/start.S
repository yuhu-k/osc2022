.section ".text.boot"

.global _start

_start:
1:
    ldr     x1, =__lr__
    str     lr, [x1]

    ldr     x1, =__stack_start
    mov     sp, x1

2:  bl      main

    ldr     x1, =__lr__
    ldr     lr, [x1]
    ret
