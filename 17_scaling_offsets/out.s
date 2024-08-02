	.text
	.data
	.globl	c
c:	.byte	0
	.data
	.globl	d
d:	.byte	0
	.data
	.globl	p
p:	.byte	0
	.data
	.globl	q
q:	.byte	0
	.data
	.globl	r
r:	.byte	0
	.data
	.globl	e
e:	.long	0
	.data
	.globl	f
f:	.byte	0
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
	mov	r4, #20
	ldr	r3, .L2+4
	strb	r4, [r3]
	mov	r4, #30
	ldr	r3, .L2+8
	strb	r4, [r3]
	mov	r4, #40
	ldr	r3, .L2+12
	strb	r4, [r3]
	mov	r4, #50
	ldr	r3, .L2+16
	strb	r4, [r3]
	ldr	r3, .L2+16
	ldrb	r4, [r3]
	mov	r0, r4
	bl	printint
	nop
	ldr	r3, .L2+0
	mov	r4, r3
	mov	r5, #2
	lsl	r5, r5, #0
	add	r5, r4, r5
	ldr	r3, .L2+20
	str	r5, [r3]
	ldr	r3, .L2+20
	ldr	r4, [r3]
	ldrb	r4, [r4]
	ldr	r3, .L2+24
	strb	r4, [r3]
	ldr	r3, .L2+24
	ldrb	r4, [r3]
	mov	r0, r4
	bl	printint
	mov	r4, r0
	ldr	r3, .L2+8
	mov	r4, r3
	mov	r5, #1
	lsl	r5, r5, #0
	sub	r4, r4, r5
	ldr	r3, .L2+20
	str	r4, [r3]
	ldr	r3, .L2+20
	ldr	r4, [r3]
	ldrb	r4, [r4]
	ldr	r3, .L2+24
	strb	r4, [r3]
	ldr	r3, .L2+24
	ldrb	r4, [r3]
	mov	r0, r4
	bl	printint
	mov	r4, r0
	mov	r4, #0
	mov	r0, r4
	b	L1
L1:
	sub	sp, fp, #4
	pop	{fp, pc}
	.align	2
.L2:
	.word c
	.word d
	.word p
	.word q
	.word r
	.word e
	.word f
.L3:
