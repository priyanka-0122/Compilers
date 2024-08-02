	.text
	.data
	.globl	c
c:	.byte	0
	.data
	.globl	d
d:	.byte	0
	.data
	.globl	e
e:	.quad	0
	.data
	.globl	f
f:	.byte	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$12, %r8
	movb	%r8b, c(%rip)
	movzbq	c(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$13, %r8
	movb	%r8b, d(%rip)
	movzbq	d(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	leaq	c(%rip), %r8
	movq	%r8, e(%rip)
	movq	e(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	leaq	c(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r10
	movq	%r10, e(%rip)
	movq	e(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
