// File author is Ítalo Lima Marconato Matias
//
// Created on December 21 of 2018, at 23:21 BRT
// Last edited on November 15 of 2019, at 22:08 BRT

.ifdef ARCH_64
.code64

.global Jump
Jump:
	mov %rcx, %rax
	mov %rdx, %rbx
	mov %r8, %rcx
	mov %r9, %rdx
	mov 0x28(%rsp), %rsi
	mov 0x30(%rsp), %rdi
	mov 0x38(%rsp), %rbp
	mov 0x40(%rsp), %r8
	jmp *%rax
.else
.code32

.global _Jump
_Jump:
	mov 4(%esp), %eax
	mov 8(%esp), %ebx
	mov 12(%esp), %ecx
	mov 16(%esp), %edx
	mov 20(%esp), %esi
	mov 24(%esp), %edi
	mov 28(%esp), %ebp
	mov 32(%esp), %esp
	jmp *%eax
.endif
