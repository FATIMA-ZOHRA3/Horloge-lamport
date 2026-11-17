	.file	"main.c"
	.text
	.section .rdata,"dr"
	.align 8
.LC0:
	.ascii "Usage : %s <numero> <n> <scenario>\12\0"
	.align 8
.LC1:
	.ascii "[P%d] Scenario charge : %d action(s)\12\0"
	.align 8
.LC2:
	.ascii "[P%d] Echec de l'initialisation\12\0"
	.align 8
.LC3:
	.ascii "[P%d] Connecte aux %d autres processus\12\0"
.LC4:
	.ascii "Scenario termine\0"
.LC5:
	.ascii "Scenario interrompu\0"
.LC6:
	.ascii "[P%d] %s\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movl	$6384, %eax
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	6384
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 6272(%rbp)
	movq	%rdx, 6280(%rbp)
	call	__main
	cmpl	$4, 6272(%rbp)
	je	.L2
	movq	6280(%rbp), %rax
	movq	(%rax), %rax
	leaq	.LC0(%rip), %rcx
	movq	%rax, %rdx
	call	printf
	movl	$1, %eax
	jmp	.L9
.L2:
	movq	6280(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	atoi
	movl	%eax, 6252(%rbp)
	movq	6280(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	atoi
	movl	%eax, 6248(%rbp)
	movq	6280(%rbp), %rax
	addq	$24, %rax
	movq	(%rax), %rax
	movl	6248(%rbp), %r8d
	movl	6252(%rbp), %edx
	leaq	sc.0(%rip), %rcx
	movl	%r8d, %r9d
	movl	%edx, %r8d
	movq	%rax, %rdx
	call	scenario_charger
	testl	%eax, %eax
	jne	.L4
	movl	$1, %eax
	jmp	.L9
.L4:
	movl	2048+sc.0(%rip), %edx
	movl	6252(%rbp), %eax
	leaq	.LC1(%rip), %rcx
	movl	%edx, %r8d
	movl	%eax, %edx
	call	printf
	call	network_init
	testl	%eax, %eax
	jne	.L5
	movl	$1, %eax
	jmp	.L9
.L5:
	movl	6248(%rbp), %ecx
	movl	6252(%rbp), %edx
	leaq	-96(%rbp), %rax
	movl	%ecx, %r8d
	movq	%rax, %rcx
	call	process_init
	testl	%eax, %eax
	jne	.L6
	movl	6252(%rbp), %eax
	leaq	.LC2(%rip), %rcx
	movl	%eax, %edx
	call	printf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	process_fermer
	call	network_cleanup
	movl	$1, %eax
	jmp	.L9
.L6:
	movl	6248(%rbp), %eax
	leal	-1(%rax), %edx
	movl	6252(%rbp), %eax
	leaq	.LC3(%rip), %rcx
	movl	%edx, %r8d
	movl	%eax, %edx
	call	printf
	leaq	-96(%rbp), %rax
	leaq	sc.0(%rip), %rcx
	movq	%rax, %rdx
	call	scenario_executer
	movl	%eax, 6244(%rbp)
	cmpl	$0, 6244(%rbp)
	je	.L7
	leaq	.LC4(%rip), %rax
	jmp	.L8
.L7:
	leaq	.LC5(%rip), %rax
.L8:
	movl	6252(%rbp), %edx
	leaq	.LC6(%rip), %rcx
	movq	%rax, %r8
	call	printf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	process_fermer
	call	network_cleanup
	cmpl	$0, 6244(%rbp)
	sete	%al
	movzbl	%al, %eax
.L9:
	addq	$6384, %rsp
	popq	%rbp
	ret
	.seh_endproc
.lcomm sc.0,2052,32
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r3) 16.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	atoi;	.scl	2;	.type	32;	.endef
	.def	scenario_charger;	.scl	2;	.type	32;	.endef
	.def	network_init;	.scl	2;	.type	32;	.endef
	.def	process_init;	.scl	2;	.type	32;	.endef
	.def	process_fermer;	.scl	2;	.type	32;	.endef
	.def	network_cleanup;	.scl	2;	.type	32;	.endef
	.def	scenario_executer;	.scl	2;	.type	32;	.endef
