	.text
	.comm	x,1,1
	.comm	y,1,1
	.text
	.globl	main
	.type	main, %function
main:
	push	{fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	mov	r4, #10
	ldr	r3, .L2+0
	strb	r4, [r3]
	mov	r4, #12
	ldr	r3, .L2+4
	strb	r4, [r3]
	ldr	r3, .L2+0
	ldrb	r4, [r3]
	ldr	r3, .L2+4
	ldrb	r5, [r3]
	add	r5, r4, r5
	mov	r0, r5
	bl	printint
	nop
L1:
	sub	sp, fp, #4
	pop	{fp, pc}
	.align	2
.L2:
	.word x
	.word y
.L3:
