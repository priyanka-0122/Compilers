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
	.comm	j,1,1
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$50, %r8
	movb	%r8b, j(%rip)
	movq	$50, %r8
	movq	%r8, i(%rip)
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movzbq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$50, %r8
	movq	%r8, i(%rip)
L1:
	movq	i(%rip), %r8
	movq	$4000, %r9
	cmpq	%r9, %r8
	jge	L2
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	i(%rip), %r8
	movq	$2, %r9
	imulq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L1
L2:
	movq	$50, %r8
	movb	%r8b, j(%rip)
L3:
	movzbq	j(%rip), %r8
	movq	$0, %r9
	cmpq	%r9, %r8
	je	L4
	movzbq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movzbq	j(%rip), %r8
	movq	$2, %r9
	imulq	%r8, %r9
	movb	%r9b, j(%rip)
	jmp	L3
L4:
movl $0, %eax
	popq	%rbp
	ret
