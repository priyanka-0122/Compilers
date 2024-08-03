	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.comm	i,8,8
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setge	%r9b
	movzbq	%r9b, %r9
	movq	%r9, i(%rip)
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
movl $0, %eax
	popq	%rbp
	ret
