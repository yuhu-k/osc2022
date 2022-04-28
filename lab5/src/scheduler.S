.global switch_to
switch_to:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp fp, lr, [x1, 16 * 5]
    ldr x9, [x1, 16 * 6]
    mov sp,  x9
    msr tpidr_el1, x1
    mov x0, #0
    ret

.global get_current
get_current:
    mrs x0, tpidr_el1
    ret

.global store_and_jump
store_and_jump:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]

    ldr x0, [x1, 8 * 2]
    ldr x2, [x1, 8 * 3]
    ldr x3, [x1, 8 * 4]

    mov x29, sp

    ldr x9, [x1, 8 * 1]
    mov sp, x9

    stp   x1, x29, [sp, #-16]!
    mov   fp, sp

    ldr x9, [x1, 8 * 0]
    msr tpidr_el1, x1
    mov x1, sp
    blr x9

    ldp  x0, x29, [sp, 16 * 0]
    mov  sp, x29

    mov  w1, 3
    str  w1, [x0, 140]

    bl   exit

.global set_current
set_current:
    ldr x9, [x0, 8 * 1]
    mov sp, x9

    mov fp, sp

    msr tpidr_el1, x0
    ldr x9, [x0, 8 * 0]
    blr  x9

.global set_proc
set_proc:
    msr tpidr_el1, x0
    ret

