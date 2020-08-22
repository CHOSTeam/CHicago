/* File author is Ítalo Lima Marconato Matias
 *
 * Created on August 11 of 2020, at 13:16 BRT
 * Last edited on August 11 of 2020, at 13:20 BRT */

.ifdef ARCH_64
.section .init
	pop %rbp
	ret

.section .fini
	pop %rbp
	ret
.else
.section .init
	pop %ebp
	ret

.section .fini
	pop %ebp
	ret
.endif
