	.text
	.comm	a,4,4
	.comm	b,4,4
	.comm	a,4,4
	.text
	.globl	add
	.type	add, @function
add:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$2, %r8
	movl	%r8d, a(%rip)
	movq	$10, %r8
	movl	%r8d, b(%rip)
	movzb	b(%rip), %r8
	movzb	a(%rip), %r9
	addq	%r8, %r9
	movl	%r9d, b(%rip)
	movzb	b(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movzb	b(%rip), %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
	.comm	b,4,4
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$3, %r8
	movl	%r8d, b(%rip)
	movq	$5, %r8
	movl	%r8d, a(%rip)
	movzb	b(%rip), %r8
	movzb	a(%rip), %r9
	addq	%r8, %r9
	movl	%r9d, b(%rip)
	movzb	b(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movzb	b(%rip), %r8
	movq	%r8, %rdi
	call	add
	movq	%rax, %r9
	movzb	b(%rip), %r8
	addq	%r9, %r8
	movl	%r8d, b(%rip)
	movzb	b(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movzb	a(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L2
L2:
	popq %rbp
	ret
