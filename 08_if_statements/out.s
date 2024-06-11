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

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq %rsp, %rbp
	movq	$1, %r8
	movq	$10, %r9
	cmpq	%r9, %r8
	jge	L1
	movq	$10, %r8
	movq	%r8, %rdi
	call	printint
	movq	$10, %r8
	movq	$20, %r9
	cmpq	%r9, %r8
	jge	L3
	movq	$20, %r8
	movq	%r8, %rdi
	call	printint
L3:
	jmp	L2
L1:
	movq	$40, %r8
	movq	%r8, %rdi
	call	printint
L2:
	movl $0, %eax
	popq %rbp
	ret
