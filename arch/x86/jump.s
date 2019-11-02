// File author is Ítalo Lima Marconato Matias
//
// Created on December 21 of 2018, at 23:21 BRT
// Last edited on October 28 of 2019, at 16:52 BRT

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
	jmp *%rax
.else
.code32

.global _Jump
_Jump:
	mov 24(%esp), %edi
	mov 20(%esp), %esi
	mov 16(%esp), %edx
	mov 12(%esp), %ecx
	mov 8(%esp), %ebx
	mov 4(%esp), %eax
	jmp *%eax
.endif
