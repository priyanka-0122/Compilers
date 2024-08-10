	.text
	.data
	.globl	x
x:	.long	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setl	%r9b
	movzbq	%r9b, %r9
	movl	%r9d, x(%rip)
	movl	x(%rip), %r8d
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
L1:
	popq %rbp
	ret
