/* File author is Ítalo Lima Marconato Matias
 *
 * Created on August 11 of 2020, at 13:16 BRT
 * Last edited on February 06 of 2021, at 10:54 BRT */

/* We need to manually create the crti/n files, as, unlike arm64, the baremetal x86 and amd64 compiler doesn't include
 * those files. */

#ifdef __i386__
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
#else
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
#endif
