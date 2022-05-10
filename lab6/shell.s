	.arch armv8-a
	.file	"shell.c"
	.text
	.global	cpio_start
	.bss
	.align	2
	.type	cpio_start, %object
	.size	cpio_start, 4
cpio_start:
	.zero	4
	.global	cpio_end
	.align	2
	.type	cpio_end, %object
	.size	cpio_end, 4
cpio_end:
	.zero	4
	.global	cmd_buffer
	.align	3
	.type	cmd_buffer, %object
	.size	cmd_buffer, 1024
cmd_buffer:
	.zero	1024
	.global	cmd_index
	.align	2
	.type	cmd_index, %object
	.size	cmd_index, 4
cmd_index:
	.zero	4
	.global	cmd_flag
	.align	2
	.type	cmd_flag, %object
	.size	cmd_flag, 4
cmd_flag:
	.zero	4
	.section	.rodata
	.align	3
.LC0:
	.string	"\n\n\nHello From RPI3\n"
	.align	3
.LC1:
	.string	"linux,initrd-start"
	.string	""
	.align	3
.LC2:
	.string	"/chosen"
	.string	""
	.align	3
.LC3:
	.string	"linux,initrd-end"
	.string	""
	.align	3
.LC4:
	.string	"Ramf start: 0x%x\n"
	.align	3
.LC5:
	.string	"Ramf end: 0x%x\n"
	.text
	.align	2
	.global	shell_init
	.type	shell_init, %function
shell_init:
.LFB0:
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	mov	x29, sp
	adrp	x0, :got:__heap_start
	ldr	x0, [x0, #:got_lo12:__heap_start]
	sub	x0, x0, #8
	str	x0, [sp, 16]
	ldr	x0, [sp, 16]
	str	wzr, [x0]
	bl	uart_init
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	bl	uart_init_buffer
	bl	uart_flush
	bl	core_timer_init
	bl	init_allocator
	adrp	x0, .LC1
	add	x1, x0, :lo12:.LC1
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	find_property_value
	str	x0, [sp, 24]
	adrp	x0, .LC3
	add	x1, x0, :lo12:.LC3
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	find_property_value
	str	x0, [sp, 32]
	ldr	x0, [sp, 24]
	cmp	x0, 0
	beq	.L2
	ldr	x0, [sp, 24]
	ldr	w0, [x0]
	bl	letobe
	mov	w1, w0
	adrp	x0, .LC4
	add	x0, x0, :lo12:.LC4
	bl	uart_printf
	ldr	x0, [sp, 24]
	ldr	w0, [x0]
	bl	letobe
	mov	w1, w0
	adrp	x0, cpio_start
	add	x0, x0, :lo12:cpio_start
	str	w1, [x0]
.L2:
	ldr	x0, [sp, 32]
	cmp	x0, 0
	beq	.L3
	ldr	x0, [sp, 32]
	ldr	w0, [x0]
	bl	letobe
	mov	w1, w0
	adrp	x0, .LC5
	add	x0, x0, :lo12:.LC5
	bl	uart_printf
	ldr	x0, [sp, 32]
	ldr	w0, [x0]
	bl	letobe
	mov	w1, w0
	adrp	x0, cpio_end
	add	x0, x0, :lo12:cpio_end
	str	w1, [x0]
.L3:
	mov	w1, 4096
	mov	w0, 0
	bl	memory_reserve
	adrp	x0, cpio_start
	add	x0, x0, :lo12:cpio_start
	ldr	w2, [x0]
	adrp	x0, cpio_end
	add	x0, x0, :lo12:cpio_end
	ldr	w0, [x0]
	mov	w1, w0
	mov	w0, w2
	bl	memory_reserve
	adrp	x0, :got:_begin_
	ldr	x0, [x0, #:got_lo12:_begin_]
	mov	w2, w0
	adrp	x0, :got:_end_
	ldr	x0, [x0, #:got_lo12:_end_]
	mov	w1, w0
	mov	w0, w2
	bl	memory_reserve
	adrp	x0, :got:__dtb_addr
	ldr	x0, [x0, #:got_lo12:__dtb_addr]
	str	x0, [sp, 40]
	ldr	x0, [sp, 40]
	ldr	w2, [x0]
	ldr	x0, [sp, 40]
	ldr	w0, [x0]
	add	w0, w0, 1048576
	mov	w1, w0
	mov	w0, w2
	bl	memory_reserve
	mov	w1, 524288
	mov	w0, 0
	bl	memory_reserve
	bl	init_thread
	nop
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	shell_init, .-shell_init
	.align	2
	.global	reset_flag
	.type	reset_flag, %function
reset_flag:
.LFB1:
	.cfi_startproc
	stp	x29, x30, [sp, -16]!
	.cfi_def_cfa_offset 16
	.cfi_offset 29, -16
	.cfi_offset 30, -8
	mov	x29, sp
	adrp	x0, cmd_flag
	add	x0, x0, :lo12:cmd_flag
	str	wzr, [x0]
	mov	x0, 20548
	movk	x0, 0x3f21, lsl 16
	mov	w1, 1
	str	w1, [x0]
	bl	core_timer_disable
	bl	irq_enable
	nop
	ldp	x29, x30, [sp], 16
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE1:
	.size	reset_flag, .-reset_flag
	.section	.rodata
	.align	3
.LC6:
	.string	"# "
	.align	3
.LC7:
	.string	"\n"
	.text
	.align	2
	.global	uart_read_line
	.type	uart_read_line, %function
uart_read_line:
.LFB2:
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	mov	x29, sp
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [x0]
	str	x1, [sp, 40]
	mov	x1, 0
	adrp	x0, cmd_flag
	add	x0, x0, :lo12:cmd_flag
	ldr	w0, [x0]
	cmp	w0, 0
	bne	.L9
	adrp	x0, .LC6
	add	x0, x0, :lo12:.LC6
	bl	uart_printf
	adrp	x0, cmd_flag
	add	x0, x0, :lo12:cmd_flag
	mov	w1, 1
	str	w1, [x0]
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	str	wzr, [x0]
	str	wzr, [sp, 24]
	b	.L7
.L8:
	adrp	x0, cmd_buffer
	add	x1, x0, :lo12:cmd_buffer
	ldrsw	x0, [sp, 24]
	strb	wzr, [x1, x0]
	ldr	w0, [sp, 24]
	add	w0, w0, 1
	str	w0, [sp, 24]
.L7:
	ldr	w0, [sp, 24]
	cmp	w0, 1023
	ble	.L8
	b	.L9
.L17:
	adrp	x0, cmd_flag
	add	x0, x0, :lo12:cmd_flag
	ldr	w0, [x0]
	cmp	w0, 0
	bne	.L10
	adrp	x0, .LC6
	add	x0, x0, :lo12:.LC6
	bl	uart_printf
	adrp	x0, cmd_flag
	add	x0, x0, :lo12:cmd_flag
	mov	w1, 1
	str	w1, [x0]
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	str	wzr, [x0]
	str	wzr, [sp, 28]
	b	.L11
.L12:
	adrp	x0, cmd_buffer
	add	x1, x0, :lo12:cmd_buffer
	ldrsw	x0, [sp, 28]
	strb	wzr, [x1, x0]
	ldr	w0, [sp, 28]
	add	w0, w0, 1
	str	w0, [sp, 28]
.L11:
	ldr	w0, [sp, 28]
	cmp	w0, 1023
	ble	.L12
.L10:
	ldrb	w0, [sp, 23]
	cmp	w0, 13
	bne	.L13
	adrp	x0, .LC7
	add	x0, x0, :lo12:.LC7
	bl	uart_printf
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	ldr	w0, [x0]
	add	w2, w0, 1
	adrp	x1, cmd_index
	add	x1, x1, :lo12:cmd_index
	str	w2, [x1]
	adrp	x1, cmd_buffer
	add	x1, x1, :lo12:cmd_buffer
	uxtw	x0, w0
	strb	wzr, [x1, x0]
	adrp	x0, cmd_flag
	add	x0, x0, :lo12:cmd_flag
	str	wzr, [x0]
	adrp	x0, cmd_buffer
	add	x0, x0, :lo12:cmd_buffer
	bl	check
	b	.L9
.L13:
	ldrb	w0, [sp, 23]
	cmp	w0, 8
	beq	.L14
	ldrb	w0, [sp, 23]
	cmp	w0, 127
	bne	.L15
.L14:
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	ldr	w0, [x0]
	cmp	w0, 0
	beq	.L9
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	ldr	w0, [x0]
	sub	w1, w0, #1
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	str	w1, [x0]
	mov	w0, 8
	strb	w0, [sp, 32]
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	ldr	w2, [x0]
	adrp	x0, cmd_buffer
	add	x1, x0, :lo12:cmd_buffer
	uxtw	x0, w2
	strb	wzr, [x1, x0]
	mov	w0, 8
	bl	uart_write
	mov	w0, 32
	bl	uart_write
	mov	w0, 8
	bl	uart_write
	b	.L9
.L15:
	ldrb	w0, [sp, 23]
	cmp	w0, 31
	bls	.L9
	ldrb	w0, [sp, 23]
	cmp	w0, 126
	bhi	.L9
	adrp	x0, cmd_index
	add	x0, x0, :lo12:cmd_index
	ldr	w0, [x0]
	add	w2, w0, 1
	adrp	x1, cmd_index
	add	x1, x1, :lo12:cmd_index
	str	w2, [x1]
	ldrb	w2, [sp, 23]
	adrp	x1, cmd_buffer
	add	x1, x1, :lo12:cmd_buffer
	uxtw	x0, w0
	strb	w2, [x1, x0]
	ldrb	w0, [sp, 23]
	bl	uart_push
	mov	x0, 20548
	movk	x0, 0x3f21, lsl 16
	ldr	w1, [x0]
	mov	x0, 20548
	movk	x0, 0x3f21, lsl 16
	orr	w1, w1, 2
	str	w1, [x0]
.L9:
	add	x0, sp, 23
	bl	uart_pop
	cmp	w0, 0
	bne	.L17
	nop
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [sp, 40]
	ldr	x2, [x0]
	subs	x1, x1, x2
	mov	x2, 0
	beq	.L18
	bl	__stack_chk_fail
.L18:
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE2:
	.size	uart_read_line, .-uart_read_line
	.global	m
	.bss
	.align	3
	.type	m, %object
	.size	m, 80
m:
	.zero	80
	.text
	.align	2
	.global	parse_command
	.type	parse_command, %function
parse_command:
.LFB3:
	.cfi_startproc
	stp	x29, x30, [sp, -80]!
	.cfi_def_cfa_offset 80
	.cfi_offset 29, -80
	.cfi_offset 30, -72
	mov	x29, sp
	str	x0, [sp, 24]
	mov	w0, 128
	bl	malloc
	str	x0, [sp, 56]
	str	wzr, [sp, 44]
	ldr	x0, [sp, 24]
	ldrb	w0, [x0]
	cmp	w0, 0
	bne	.L20
	mov	x0, 0
	b	.L21
.L20:
	str	wzr, [sp, 48]
	b	.L22
.L29:
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 32
	beq	.L30
	mov	w0, 128
	bl	malloc
	str	x0, [sp, 64]
	str	wzr, [sp, 52]
	b	.L25
.L27:
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x1, x1, x0
	ldr	w0, [sp, 52]
	add	w2, w0, 1
	str	w2, [sp, 52]
	sxtw	x0, w0
	ldr	x2, [sp, 64]
	add	x0, x2, x0
	ldrb	w1, [x1]
	strb	w1, [x0]
	ldr	w0, [sp, 48]
	add	w0, w0, 1
	str	w0, [sp, 48]
.L25:
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 0
	beq	.L26
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 32
	beq	.L26
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 31
	bls	.L26
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	sxtb	w0, w0
	cmp	w0, 0
	bge	.L27
.L26:
	ldrsw	x0, [sp, 52]
	ldr	x1, [sp, 64]
	add	x0, x1, x0
	strb	wzr, [x0]
	ldr	w0, [sp, 44]
	add	w1, w0, 1
	str	w1, [sp, 44]
	sxtw	x0, w0
	lsl	x0, x0, 3
	ldr	x1, [sp, 56]
	add	x0, x1, x0
	ldr	x1, [sp, 64]
	str	x1, [x0]
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 0
	beq	.L31
	b	.L24
.L30:
	nop
.L24:
	ldr	w0, [sp, 48]
	add	w0, w0, 1
	str	w0, [sp, 48]
.L22:
	ldrsw	x0, [sp, 48]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 0
	bne	.L29
	b	.L28
.L31:
	nop
.L28:
	mov	w0, 16
	bl	malloc
	str	x0, [sp, 72]
	ldr	x0, [sp, 72]
	ldr	w1, [sp, 44]
	str	w1, [x0, 8]
	ldr	x0, [sp, 72]
	ldr	x1, [sp, 56]
	str	x1, [x0]
	ldr	x0, [sp, 72]
.L21:
	ldp	x29, x30, [sp], 80
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE3:
	.size	parse_command, .-parse_command
	.section	.rodata
	.align	3
.LC8:
	.string	"10\n"
	.align	3
.LC9:
	.string	"20\n"
	.text
	.align	2
	.global	temp_func
	.type	temp_func, %function
temp_func:
.LFB4:
	.cfi_startproc
	stp	x29, x30, [sp, -16]!
	.cfi_def_cfa_offset 16
	.cfi_offset 29, -16
	.cfi_offset 30, -8
	mov	x29, sp
	adrp	x0, .LC8
	add	x0, x0, :lo12:.LC8
	bl	uart_printf
	mov	w0, 5000
	bl	delay
	adrp	x0, .LC9
	add	x0, x0, :lo12:.LC9
	bl	uart_printf
	nop
	ldp	x29, x30, [sp], 16
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE4:
	.size	temp_func, .-temp_func
	.align	2
	.global	temp_func2
	.type	temp_func2, %function
temp_func2:
.LFB5:
	.cfi_startproc
	stp	x29, x30, [sp, -16]!
	.cfi_def_cfa_offset 16
	.cfi_offset 29, -16
	.cfi_offset 30, -8
	mov	x29, sp
	adrp	x0, .LC8
	add	x0, x0, :lo12:.LC8
	bl	uart_printf
	mov	w0, 10000
	bl	delay
	adrp	x0, .LC9
	add	x0, x0, :lo12:.LC9
	bl	uart_printf
	nop
	ldp	x29, x30, [sp], 16
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE5:
	.size	temp_func2, .-temp_func2
	.section	.rodata
	.align	3
.LC10:
	.string	"\nFork Test, pid %d\n"
	.align	3
.LC11:
	.string	"first child pid: %d, cnt: %d, ptr: %x, sp : %x\n"
	.align	3
.LC12:
	.string	"second child pid: %d, cnt: %d, ptr: %x, sp : %x\n"
	.align	3
.LC13:
	.string	"parent here, pid %d, child %d\n"
	.text
	.align	2
	.global	fork_test
	.type	fork_test, %function
fork_test:
.LFB6:
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	mov	x29, sp
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [x0]
	str	x1, [sp, 40]
	mov	x1, 0
	bl	getpid1
	mov	w1, w0
	adrp	x0, .LC10
	add	x0, x0, :lo12:.LC10
	bl	uart_printf
	mov	w0, 1
	str	w0, [sp, 24]
	str	wzr, [sp, 28]
	bl	fork1
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 0
	bne	.L35
#APP
// 150 "src/shell.c" 1
	mov x0, sp
// 0 "" 2
#NO_APP
	str	x0, [sp, 32]
	bl	getpid1
	mov	w5, w0
	ldr	w0, [sp, 24]
	add	x1, sp, 24
	ldr	x4, [sp, 32]
	mov	x3, x1
	mov	w2, w0
	mov	w1, w5
	adrp	x0, .LC11
	add	x0, x0, :lo12:.LC11
	bl	uart_printf
	ldr	w0, [sp, 24]
	add	w0, w0, 1
	str	w0, [sp, 24]
	bl	fork1
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 0
	beq	.L38
#APP
// 155 "src/shell.c" 1
	mov x0, sp
// 0 "" 2
#NO_APP
	str	x0, [sp, 32]
	bl	getpid1
	mov	w5, w0
	ldr	w0, [sp, 24]
	add	x1, sp, 24
	ldr	x4, [sp, 32]
	mov	x3, x1
	mov	w2, w0
	mov	w1, w5
	adrp	x0, .LC11
	add	x0, x0, :lo12:.LC11
	bl	uart_printf
	b	.L37
.L39:
#APP
// 160 "src/shell.c" 1
	mov x0, sp
// 0 "" 2
#NO_APP
	str	x0, [sp, 32]
	bl	getpid1
	mov	w5, w0
	ldr	w0, [sp, 24]
	add	x1, sp, 24
	ldr	x4, [sp, 32]
	mov	x3, x1
	mov	w2, w0
	mov	w1, w5
	adrp	x0, .LC12
	add	x0, x0, :lo12:.LC12
	bl	uart_printf
	mov	w0, 5000
	bl	delay
	ldr	w0, [sp, 24]
	add	w0, w0, 1
	str	w0, [sp, 24]
.L38:
	ldr	w0, [sp, 24]
	cmp	w0, 4
	ble	.L39
.L37:
	bl	exit1
	b	.L42
.L35:
	bl	getpid1
	ldr	w2, [sp, 28]
	mov	w1, w0
	adrp	x0, .LC13
	add	x0, x0, :lo12:.LC13
	bl	uart_printf
.L42:
	nop
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [sp, 40]
	ldr	x2, [x0]
	subs	x1, x1, x2
	mov	x2, 0
	beq	.L41
	bl	__stack_chk_fail
.L41:
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE6:
	.size	fork_test, .-fork_test
	.section	.rodata
	.align	3
.LC14:
	.string	"123"
	.align	3
.LC16:
	.string	"How are you\n"
	.align	3
.LC17:
	.string	"%s\n"
	.align	3
.LC18:
	.string	"%d\n"
	.text
	.align	2
	.global	foo
	.type	foo, %function
foo:
.LFB7:
	.cfi_startproc
	stp	x29, x30, [sp, -160]!
	.cfi_def_cfa_offset 160
	.cfi_offset 29, -160
	.cfi_offset 30, -152
	mov	x29, sp
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [x0]
	str	x1, [sp, 152]
	mov	x1, 0
	adrp	x0, .LC14
	add	x0, x0, :lo12:.LC14
	bl	uart_printf
	adrp	x0, .LC15
	add	x1, x0, :lo12:.LC15
	add	x0, sp, 24
	ldr	x2, [x1]
	str	x2, [x0]
	ldr	x1, [x1, 5]
	str	x1, [x0, 5]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 32]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 48]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 64]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 80]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 96]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 112]
	add	x0, sp, 5
	stp	xzr, xzr, [x0, 128]
	strh	wzr, [sp, 149]
	strb	wzr, [sp, 151]
	adrp	x0, .LC16
	add	x0, x0, :lo12:.LC16
	bl	uart_printf
	add	x0, sp, 24
	mov	x1, x0
	adrp	x0, .LC17
	add	x0, x0, :lo12:.LC17
	bl	uart_printf
	mov	x1, 128
	adrp	x0, .LC18
	add	x0, x0, :lo12:.LC18
	bl	uart_printf
	add	x0, sp, 24
	mov	x1, 128
	bl	uart_write1
	bl	exit1
	nop
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [sp, 152]
	ldr	x2, [x0]
	subs	x1, x1, x2
	mov	x2, 0
	beq	.L44
	bl	__stack_chk_fail
.L44:
	ldp	x29, x30, [sp], 160
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE7:
	.size	foo, .-foo
	.section	.rodata
	.align	3
.LC15:
	.string	"How are you\n"
	.zero	115
	.text
	.section	.rodata
	.align	3
.LC19:
	.string	"help"
	.align	3
.LC20:
	.string	"help    : print the help menu\n"
	.align	3
.LC21:
	.string	"hello   : print Hello World!\n"
	.align	3
.LC22:
	.string	"reboot  : reboot the device\n"
	.align	3
.LC23:
	.string	"hello"
	.align	3
.LC24:
	.string	"Hello World!\n"
	.align	3
.LC25:
	.string	"reboot"
	.align	3
.LC26:
	.string	"Rebooting...\n"
	.align	3
.LC27:
	.string	"ls"
	.align	3
.LC28:
	.string	"cat "
	.align	3
.LC29:
	.string	"./"
	.align	3
.LC30:
	.string	"timer"
	.align	3
.LC31:
	.string	"sleep"
	.align	3
.LC32:
	.string	"thread"
	.align	3
.LC33:
	.string	":a:r"
	.align	3
.LC34:
	.string	"mem"
	.align	3
.LC35:
	.string	":s:a:p"
	.align	3
.LC36:
	.string	"lp"
	.align	3
.LC37:
	.string	"test"
	.align	3
.LC38:
	.string	"tet"
	.align	3
.LC39:
	.string	"command not found: %s\n"
	.text
	.align	2
	.global	check
	.type	check, %function
check:
.LFB8:
	.cfi_startproc
	stp	x29, x30, [sp, -240]!
	.cfi_def_cfa_offset 240
	.cfi_offset 29, -240
	.cfi_offset 30, -232
	mov	x29, sp
	str	x0, [sp, 24]
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [x0]
	str	x1, [sp, 232]
	mov	x1, 0
	ldr	x0, [sp, 24]
	bl	parse_command
	str	x0, [sp, 88]
	ldr	x0, [sp, 24]
	ldrb	w0, [x0]
	cmp	w0, 0
	beq	.L93
	ldr	x0, [sp, 24]
	ldrb	w0, [x0]
	cmp	w0, 10
	beq	.L93
	adrp	x0, .LC19
	add	x1, x0, :lo12:.LC19
	ldr	x0, [sp, 24]
	bl	strcmp
	cmp	w0, 1
	bne	.L49
	adrp	x0, .LC20
	add	x0, x0, :lo12:.LC20
	bl	uart_printf
	adrp	x0, .LC21
	add	x0, x0, :lo12:.LC21
	bl	uart_printf
	adrp	x0, .LC22
	add	x0, x0, :lo12:.LC22
	bl	uart_printf
	b	.L45
.L49:
	adrp	x0, .LC23
	add	x1, x0, :lo12:.LC23
	ldr	x0, [sp, 24]
	bl	strcmp
	cmp	w0, 1
	bne	.L50
	adrp	x0, .LC24
	add	x0, x0, :lo12:.LC24
	bl	uart_printf
	b	.L45
.L50:
	mov	w2, 6
	adrp	x0, .LC25
	add	x1, x0, :lo12:.LC25
	ldr	x0, [sp, 24]
	bl	strncmp
	cmp	w0, 1
	bne	.L51
	adrp	x0, .LC26
	add	x0, x0, :lo12:.LC26
	bl	uart_printf
	ldr	x0, [sp, 24]
	add	x0, x0, 6
	ldrb	w0, [x0]
	cmp	w0, 32
	beq	.L52
	mov	w0, 50
	bl	reset
.L53:
	b	.L53
.L52:
	str	wzr, [sp, 40]
	mov	w0, 7
	str	w0, [sp, 44]
	b	.L54
.L56:
	ldr	w1, [sp, 40]
	mov	w0, w1
	lsl	w0, w0, 2
	add	w0, w0, w1
	lsl	w0, w0, 1
	str	w0, [sp, 40]
	ldrsw	x0, [sp, 44]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	sub	w0, w0, #48
	ldr	w1, [sp, 40]
	add	w0, w1, w0
	str	w0, [sp, 40]
	ldr	w0, [sp, 44]
	add	w0, w0, 1
	str	w0, [sp, 44]
.L54:
	ldrsw	x0, [sp, 44]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 57
	bhi	.L55
	ldrsw	x0, [sp, 44]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 47
	bhi	.L56
.L55:
	ldr	w2, [sp, 40]
	ldr	w1, [sp, 40]
	mov	w0, 50
	cmp	w2, 50
	csel	w0, w1, w0, ge
	bl	reset
.L57:
	b	.L57
.L51:
	adrp	x0, .LC27
	add	x1, x0, :lo12:.LC27
	ldr	x0, [sp, 24]
	bl	strcmp
	cmp	w0, 0
	beq	.L58
	adrp	x0, cpio_start
	add	x0, x0, :lo12:cpio_start
	ldr	w0, [x0]
	bl	list
	b	.L45
.L58:
	mov	w2, 4
	adrp	x0, .LC28
	add	x1, x0, :lo12:.LC28
	ldr	x0, [sp, 24]
	bl	strncmp
	cmp	w0, 0
	beq	.L59
	str	wzr, [sp, 48]
	b	.L60
.L61:
	ldrsw	x0, [sp, 48]
	add	x1, sp, 104
	strb	wzr, [x1, x0]
	ldr	w0, [sp, 48]
	add	w0, w0, 1
	str	w0, [sp, 48]
.L60:
	ldr	w0, [sp, 48]
	cmp	w0, 127
	ble	.L61
	mov	w0, 4
	str	w0, [sp, 52]
	mov	w0, 4
	str	w0, [sp, 52]
	b	.L62
.L64:
	ldrsw	x0, [sp, 52]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldr	w1, [sp, 52]
	sub	w1, w1, #4
	ldrb	w2, [x0]
	sxtw	x0, w1
	add	x1, sp, 104
	strb	w2, [x1, x0]
	ldr	w0, [sp, 52]
	add	w0, w0, 1
	str	w0, [sp, 52]
.L62:
	ldrsw	x0, [sp, 52]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 45
	bls	.L63
	ldrsw	x0, [sp, 52]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 122
	bhi	.L63
	ldr	w0, [sp, 52]
	cmp	w0, 127
	bgt	.L63
	ldrsw	x0, [sp, 52]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 0
	bne	.L64
.L63:
	ldrsw	x0, [sp, 52]
	add	x1, sp, 104
	strb	wzr, [x1, x0]
	adrp	x0, cpio_start
	add	x0, x0, :lo12:cpio_start
	ldr	w1, [x0]
	add	x0, sp, 104
	bl	print_content
	b	.L45
.L59:
	mov	w2, 2
	adrp	x0, .LC29
	add	x1, x0, :lo12:.LC29
	ldr	x0, [sp, 24]
	bl	strncmp
	cmp	w0, 0
	beq	.L65
	str	wzr, [sp, 56]
	b	.L66
.L67:
	ldrsw	x0, [sp, 56]
	add	x1, sp, 104
	strb	wzr, [x1, x0]
	ldr	w0, [sp, 56]
	add	w0, w0, 1
	str	w0, [sp, 56]
.L66:
	ldr	w0, [sp, 56]
	cmp	w0, 127
	ble	.L67
	mov	w0, 2
	str	w0, [sp, 60]
	b	.L68
.L70:
	ldrsw	x0, [sp, 60]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldr	w1, [sp, 60]
	sub	w1, w1, #2
	ldrb	w2, [x0]
	sxtw	x0, w1
	add	x1, sp, 104
	strb	w2, [x1, x0]
	ldr	w0, [sp, 60]
	add	w0, w0, 1
	str	w0, [sp, 60]
.L68:
	ldrsw	x0, [sp, 60]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 45
	bls	.L69
	ldrsw	x0, [sp, 60]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 122
	bhi	.L69
	ldr	w0, [sp, 60]
	cmp	w0, 127
	bgt	.L69
	ldrsw	x0, [sp, 60]
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 0
	bne	.L70
.L69:
	add	x0, sp, 104
	str	x0, [sp, 96]
	add	x1, sp, 96
	add	x0, sp, 104
	bl	execute
	b	.L45
.L65:
	adrp	x0, .LC30
	add	x1, x0, :lo12:.LC30
	ldr	x0, [sp, 24]
	bl	strcmp
	cmp	w0, 0
	beq	.L71
#APP
// 233 "src/shell.c" 1
	mrs x1, cntfrq_el0
mrs x0, cntp_tval_el0

// 0 "" 2
#NO_APP
	str	w1, [sp, 80]
	str	w0, [sp, 84]
	ldr	w1, [sp, 84]
	ldr	w0, [sp, 80]
	sdiv	w0, w1, w0
	mov	w1, w0
	adrp	x0, .LC18
	add	x0, x0, :lo12:.LC18
	bl	uart_printf
	b	.L45
.L71:
	mov	w2, 5
	adrp	x0, .LC31
	add	x1, x0, :lo12:.LC31
	ldr	x0, [sp, 24]
	bl	strncmp
	cmp	w0, 0
	beq	.L72
	str	wzr, [sp, 64]
	b	.L73
.L75:
	ldrsw	x0, [sp, 64]
	add	x0, x0, 6
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w2, [x0]
	ldrsw	x0, [sp, 64]
	add	x1, sp, 104
	strb	w2, [x1, x0]
	ldr	w0, [sp, 64]
	add	w0, w0, 1
	str	w0, [sp, 64]
.L73:
	ldr	w0, [sp, 64]
	cmp	w0, 4
	bgt	.L74
	ldrsw	x0, [sp, 64]
	add	x0, x0, 6
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	cmp	w0, 31
	bls	.L74
	ldrsw	x0, [sp, 64]
	add	x0, x0, 6
	ldr	x1, [sp, 24]
	add	x0, x1, x0
	ldrb	w0, [x0]
	sxtb	w0, w0
	cmp	w0, 0
	bge	.L75
.L74:
	add	x0, sp, 104
	bl	atoi
	str	w0, [sp, 76]
	ldr	w0, [sp, 76]
	bl	sleep
	b	.L45
.L72:
	ldr	x0, [sp, 88]
	ldr	x0, [x0]
	ldr	x2, [x0]
	adrp	x0, .LC32
	add	x1, x0, :lo12:.LC32
	mov	x0, x2
	bl	strcmp
	cmp	w0, 0
	beq	.L76
	mov	w0, 1
	str	w0, [sp, 68]
	adrp	x0, :got:optind
	ldr	x0, [x0, #:got_lo12:optind]
	mov	w1, 1
	str	w1, [x0]
	b	.L77
.L80:
	ldr	x0, [sp, 88]
	ldr	w3, [x0, 8]
	ldr	x0, [sp, 88]
	ldr	x1, [x0]
	adrp	x0, .LC33
	add	x2, x0, :lo12:.LC33
	mov	w0, w3
	bl	getopt
	strb	w0, [sp, 39]
	ldrb	w0, [sp, 39]
	cmp	w0, 114
	beq	.L77
	cmp	w0, 114
	bgt	.L77
	cmp	w0, 0
	beq	.L79
	cmp	w0, 97
	bne	.L77
	adrp	x0, temp_func
	add	x0, x0, :lo12:temp_func
	bl	Thread
	adrp	x0, temp_func2
	add	x0, x0, :lo12:temp_func2
	bl	Thread
	b	.L77
.L79:
	str	wzr, [sp, 68]
	nop
.L77:
	ldr	w0, [sp, 68]
	cmp	w0, 0
	bne	.L80
	b	.L45
.L76:
	ldr	x0, [sp, 88]
	ldr	x0, [x0]
	ldr	x2, [x0]
	adrp	x0, .LC34
	add	x1, x0, :lo12:.LC34
	mov	x0, x2
	bl	strcmp
	cmp	w0, 0
	beq	.L81
	mov	w0, 1
	str	w0, [sp, 72]
	adrp	x0, :got:optind
	ldr	x0, [x0, #:got_lo12:optind]
	mov	w1, 1
	str	w1, [x0]
	b	.L82
.L88:
	ldr	x0, [sp, 88]
	ldr	w3, [x0, 8]
	ldr	x0, [sp, 88]
	ldr	x1, [x0]
	adrp	x0, .LC35
	add	x2, x0, :lo12:.LC35
	mov	w0, w3
	bl	getopt
	strb	w0, [sp, 38]
	ldrb	w0, [sp, 38]
	cmp	w0, 115
	beq	.L83
	cmp	w0, 115
	bgt	.L84
	cmp	w0, 112
	beq	.L85
	cmp	w0, 112
	bgt	.L84
	cmp	w0, 0
	beq	.L86
	cmp	w0, 97
	beq	.L87
	b	.L84
.L83:
	bl	pool_status
	b	.L82
.L87:
	bl	printf_thread
	b	.L82
.L85:
	bl	print_node
	b	.L82
.L86:
	str	wzr, [sp, 72]
	b	.L82
.L84:
	bl	show_status
	nop
.L82:
	ldr	w0, [sp, 72]
	cmp	w0, 0
	bne	.L88
	b	.L45
.L81:
	ldr	x0, [sp, 88]
	ldr	x0, [x0]
	ldr	x2, [x0]
	adrp	x0, .LC36
	add	x1, x0, :lo12:.LC36
	mov	x0, x2
	bl	strcmp
	cmp	w0, 0
	beq	.L89
	bl	loadimg
	b	.L45
.L89:
	ldr	x0, [sp, 88]
	ldr	x0, [x0]
	ldr	x2, [x0]
	adrp	x0, .LC37
	add	x1, x0, :lo12:.LC37
	mov	x0, x2
	bl	strcmp
	cmp	w0, 0
	beq	.L90
	adrp	x0, fork_test
	add	x0, x0, :lo12:fork_test
	bl	Thread
	b	.L45
.L90:
	ldr	x0, [sp, 88]
	ldr	x0, [x0]
	ldr	x2, [x0]
	adrp	x0, .LC38
	add	x1, x0, :lo12:.LC38
	mov	x0, x2
	bl	strcmp
	cmp	w0, 0
	beq	.L91
	adrp	x0, foo
	add	x0, x0, :lo12:foo
	bl	Thread
	b	.L45
.L91:
	ldr	x1, [sp, 24]
	adrp	x0, .LC39
	add	x0, x0, :lo12:.LC39
	bl	uart_printf
	b	.L45
.L93:
	nop
.L45:
	adrp	x0, :got:__stack_chk_guard
	ldr	x0, [x0, #:got_lo12:__stack_chk_guard]
	ldr	x1, [sp, 232]
	ldr	x2, [x0]
	subs	x1, x1, x2
	mov	x2, 0
	beq	.L92
	bl	__stack_chk_fail
.L92:
	ldp	x29, x30, [sp], 240
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE8:
	.size	check, .-check
	.ident	"GCC: (Ubuntu 10.3.0-1ubuntu1~20.04) 10.3.0"
	.section	.note.GNU-stack,"",@progbits
