.section ".text.boot"

.global _start

_start:
2:  
    ldr     x1, =__sp__
    mov     x0, sp
    str     x0, [x1]
    
    ldr     x1, =__lr__
    str     lr, [x1]
    // set stack pointer
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_size
3:  cbz     x2, 4f
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, 3b


4:  bl      main
    
    ldr     x1, =__sp__
    ldr     x0, [x1]
    mov     sp, x0

    ldr     x1, =__lr__
    ldr     lr, [x1]
    ret     lr

