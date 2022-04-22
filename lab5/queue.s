	.arch armv8-a
	.file	"queue.c"
	.text
	.global	run_queue
	.bss
	.align	3
	.type	run_queue, %object
	.size	run_queue, 8
run_queue:
	.zero	8
	.global	wait_queue
	.align	3
	.type	wait_queue, %object
	.size	wait_queue, 8
wait_queue:
	.zero	8
	.section	.rodata
	.align	3
.LC0:
	.string	"  tid: %d\n  sp: 0x%x\n  pc: 0x%x\n"
	.align	3
.LC1:
	.string	"  lr addr: 0x%x\n"
	.align	3
.LC2:
	.string	"0x%x\n"
	.align	3
.LC3:
	.string	"tid: %d\nsp: 0x%x\npc: 0x%x\n"
	.align	3
.LC4:
	.string	"lr addr: 0x%x\n"
	.text
	.align	2
	.global	schedule
	.type	schedule, %function
schedule:
.LFB0:
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	ldr	x0, [x0]
	cmp	x0, 0
	bne	.L2
	bl	push_first_thread
.L2:
	bl	get_current
	str	x0, [sp, 32]
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	ldr	x0, [x0]
	str	x0, [sp, 40]
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	ldr	x0, [x0]
	ldr	x1, [x0, 120]
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	str	x1, [x0]
	ldr	x0, [sp, 40]
	ldr	w0, [x0, 140]
	cmp	w0, 1
	bne	.L3
	ldr	x1, [sp, 40]
	ldr	x0, [sp, 32]
	bl	switch_to
	b	.L4
.L3:
	ldr	x0, [sp, 40]
	mov	w1, 1
	str	w1, [x0, 140]
	ldr	x0, [sp, 40]
	ldr	w0, [x0, 132]
	cmp	w0, 0
	bne	.L5
	ldr	x0, [sp, 40]
	bl	set_current
	b	.L4
.L5:
#APP
// 24 "src/queue.c" 1
	adr x0, .

// 0 "" 2
#NO_APP
	str	x0, [sp, 48]
#APP
// 26 "src/queue.c" 1
	mov x0, sp

// 0 "" 2
#NO_APP
	str	x0, [sp, 56]
	ldr	x0, [sp, 32]
	ldr	w0, [x0, 132]
	ldr	x3, [sp, 48]
	ldr	x2, [sp, 56]
	mov	w1, w0
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	ldr	x0, [sp, 32]
	ldr	x0, [x0, 88]
	mov	x1, x0
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	bl	uart_printf
	str	wzr, [sp, 24]
	b	.L6
.L7:
	ldrsw	x0, [sp, 24]
	lsl	x0, x0, 3
	ldr	x1, [sp, 56]
	add	x0, x1, x0
	ldr	x0, [x0]
	mov	x1, x0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	uart_printf
	ldr	w0, [sp, 24]
	add	w0, w0, 1
	str	w0, [sp, 24]
.L6:
	ldr	w0, [sp, 24]
	cmp	w0, 63
	ble	.L7
	ldr	x0, [sp, 40]
	mov	x1, x0
	ldr	x0, [sp, 32]
	bl	store_and_jump
#APP
// 33 "src/queue.c" 1
	adr x0, .

// 0 "" 2
#NO_APP
	str	x0, [sp, 56]
#APP
// 35 "src/queue.c" 1
	mov x0, sp

// 0 "" 2
#NO_APP
	str	x0, [sp, 56]
	ldr	x0, [sp, 32]
	ldr	w0, [x0, 132]
	ldr	x3, [sp, 48]
	ldr	x2, [sp, 56]
	mov	w1, w0
	adrp	x0, .LC3
	add	x0, x0, :lo12:.LC3
	bl	uart_printf
	ldr	x0, [sp, 32]
	ldr	x0, [x0, 88]
	mov	x1, x0
	adrp	x0, .LC4
	add	x0, x0, :lo12:.LC4
	bl	uart_printf
	str	wzr, [sp, 28]
	b	.L8
.L9:
	ldrsw	x0, [sp, 28]
	lsl	x0, x0, 3
	ldr	x1, [sp, 56]
	add	x0, x1, x0
	ldr	x0, [x0]
	mov	x1, x0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	uart_printf
	ldr	w0, [sp, 28]
	add	w0, w0, 1
	str	w0, [sp, 28]
.L8:
	ldr	w0, [sp, 28]
	cmp	w0, 63
	ble	.L9
.L4:
	nop
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	schedule, .-schedule
	.align	2
	.global	push2run_queue
	.type	push2run_queue, %function
push2run_queue:
.LFB1:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	str	x0, [sp, 8]
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	ldr	x0, [x0]
	cmp	x0, 0
	beq	.L11
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	ldr	x0, [x0]
	str	x0, [sp, 24]
	b	.L12
.L13:
	ldr	x0, [sp, 24]
	ldr	x0, [x0, 120]
	str	x0, [sp, 24]
.L12:
	ldr	x0, [sp, 24]
	ldr	x0, [x0, 120]
	cmp	x0, 0
	bne	.L13
	ldr	x0, [sp, 24]
	ldr	x1, [sp, 8]
	str	x1, [x0, 120]
	ldr	x0, [sp, 8]
	str	xzr, [x0, 120]
	b	.L15
.L11:
	adrp	x0, run_queue
	add	x0, x0, :lo12:run_queue
	ldr	x1, [sp, 8]
	str	x1, [x0]
.L15:
	nop
	add	sp, sp, 32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE1:
	.size	push2run_queue, .-push2run_queue
	.align	2
	.global	push2waiting_queue
	.type	push2waiting_queue, %function
push2waiting_queue:
.LFB2:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	bl	get_current
	str	x0, [sp, 24]
	ldr	x0, [sp, 24]
	mov	w1, 2
	str	w1, [x0, 140]
	adrp	x0, wait_queue
	add	x0, x0, :lo12:wait_queue
	ldr	x0, [x0]
	cmp	x0, 0
	beq	.L17
	adrp	x0, wait_queue
	add	x0, x0, :lo12:wait_queue
	ldr	x0, [x0]
	str	x0, [sp, 16]
	b	.L18
.L19:
	ldr	x0, [sp, 16]
	ldr	x0, [x0, 120]
	str	x0, [sp, 16]
.L18:
	ldr	x0, [sp, 16]
	ldr	x0, [x0, 120]
	cmp	x0, 0
	bne	.L19
	ldr	x0, [sp, 16]
	ldr	x1, [sp, 24]
	str	x1, [x0, 120]
	b	.L20
.L17:
	adrp	x0, wait_queue
	add	x0, x0, :lo12:wait_queue
	ldr	x1, [sp, 24]
	str	x1, [x0]
.L20:
	ldr	x0, [sp, 24]
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE2:
	.size	push2waiting_queue, .-push2waiting_queue
	.section	.rodata
	.align	3
.LC5:
	.string	"111"
	.text
	.align	2
	.global	wakeup_queue
	.type	wakeup_queue, %function
wakeup_queue:
.LFB3:
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	mov	x29, sp
	str	x0, [sp, 24]
	adrp	x0, .LC5
	add	x0, x0, :lo12:.LC5
	bl	uart_printf
	adrp	x0, wait_queue
	add	x0, x0, :lo12:wait_queue
	ldr	x0, [x0]
	str	x0, [sp, 40]
	ldr	x0, [sp, 40]
	cmp	x0, 0
	beq	.L22
	ldr	x0, [sp, 40]
	ldr	w1, [x0, 132]
	ldr	x0, [sp, 24]
	ldr	w0, [x0, 132]
	cmp	w1, w0
	bne	.L25
	adrp	x0, wait_queue
	add	x0, x0, :lo12:wait_queue
	ldr	x0, [x0]
	ldr	x1, [x0, 120]
	adrp	x0, wait_queue
	add	x0, x0, :lo12:wait_queue
	str	x1, [x0]
	ldr	x0, [sp, 24]
	mov	w1, 1
	str	w1, [x0, 140]
	ldr	x0, [sp, 24]
	bl	push2run_queue
	b	.L22
.L27:
	ldr	x0, [sp, 40]
	ldr	x0, [x0, 120]
	ldr	w1, [x0, 132]
	ldr	x0, [sp, 24]
	ldr	w0, [x0, 132]
	cmp	w1, w0
	bne	.L26
	ldr	x0, [sp, 40]
	ldr	x0, [x0, 120]
	ldr	x1, [x0, 120]
	ldr	x0, [sp, 40]
	str	x1, [x0, 120]
	ldr	x0, [sp, 24]
	mov	w1, 1
	str	w1, [x0, 140]
	ldr	x0, [sp, 24]
	bl	push2run_queue
	b	.L22
.L26:
	ldr	x0, [sp, 40]
	ldr	x0, [x0, 120]
	str	x0, [sp, 40]
.L25:
	ldr	x0, [sp, 40]
	ldr	x0, [x0, 120]
	cmp	x0, 0
	bne	.L27
.L22:
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE3:
	.size	wakeup_queue, .-wakeup_queue
	.ident	"GCC: (Ubuntu 10.3.0-1ubuntu1~20.04) 10.3.0"
	.section	.note.GNU-stack,"",@progbits
