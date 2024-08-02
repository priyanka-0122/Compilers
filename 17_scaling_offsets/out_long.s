	.text
	.data
	.globl	c
c:	.quad	0
	.data
	.globl	d
d:	.quad	0
	.data
	.globl	e
e:	.quad	0
	.data
	.globl	f
f:	.quad	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$12, %r8
	movq	%r8, c(%rip)
	movq	c(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$13, %r8
	movq	%r8, d(%rip)
	movq	d(%rip), %r8
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
	salq	$3, %r9
	addq	%r8, %r9
	movq	%r9, e(%rip)
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
