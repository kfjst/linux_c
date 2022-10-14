	.file	"5sec_sig.c"
	.text
	.type	alrm_handler, @function
alrm_handler:
.LFB52:
	.cfi_startproc
	movl	$0, loop(%rip)
	ret
	.cfi_endproc
.LFE52:
	.size	alrm_handler, .-alrm_handler
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB53:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$alrm_handler, %esi
	movl	$14, %edi
	call	signal
	movl	$5, %edi
	call	alarm
	movl	loop(%rip), %eax
	testl	%eax, %eax
	je	.L5
	movl	$0, %edx
.L4:
	addq	$1, %rdx
	movl	loop(%rip), %eax
	testl	%eax, %eax
	jne	.L4
	jmp	.L3
.L5:
	movl	$0, %edx
.L3:
	movl	$.LC0, %esi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE53:
	.size	main, .-main
	.data
	.align 4
	.type	loop, @object
	.size	loop, 4
loop:
	.long	1
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.12) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
