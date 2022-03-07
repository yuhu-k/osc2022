.section ".text.relocate"

.global _relocate

_relocate:
    // get cpu id
    mrs     x1, MPIDR_EL1
    and     x1, x1, #3
    cbz     x1, 2f
    // if cpu_id > 0, stop
1:
    wfe
    b       1b
    // if cpu_id == 0
2:
    // set stack pointer
    ldr     x1, =_relocate
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_size
3:  cbz     x2, 4f
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, 3b

    // jump to relocator function in C
4:  bl      relocator


.section ".text.boot"

.global _start

_start:
    // set stack pointer
    ldr     x1, =_start
    mov     sp, x1
    // jump to main function in C
1:  bl      main
    // halt this core if return
    wfe
    b       1b