/* File author is Ítalo Lima Marconato Matias
 *
 * Created on August 11 of 2020, at 13:16 BRT
 * Last edited on August 11 of 2020, at 13:19 BRT */

/* We need to manually create the crti/n files, as the default ones doesn't seem to work for some reason (probably our fault). */

.ifdef ARCH_64
.section .init
.global _init
.type _init, @function
_init:
	push %rbp
	mov %rsp, %rbp

.section .fini
.global _fini
.type _fini, @function
_fini:
	push %rbp
	mov %rsp, %rbp
.else
.section .init
.global _init
.type _init, @function
_init:
	push %ebp
	mov %esp, %ebp

.section .fini
.global _fini
.type _fini, @function
_fini:
	push %ebp
	mov %esp, %ebp
.endif
