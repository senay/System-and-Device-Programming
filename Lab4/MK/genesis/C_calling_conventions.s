	.file	"C_calling_conventions.c"
	.text
.globl g
	.type	g, @function
g:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	addl	$3, %eax
	popl	%ebp
	ret
	.size	g, .-g
.globl f
	.type	f, @function
f:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	g
	leave
	ret
	.size	f, .-f
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	$6, (%esp)
	call	f
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
