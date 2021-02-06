/* File author is Ítalo Lima Marconato Matias
 *
 * Created on August 11 of 2020, at 13:16 BRT
 * Last edited on February 06 of 2021, at 10:54 BRT */

#ifdef __i386__
.section .init
	pop %ebp
	ret

.section .fini
	pop %ebp
	ret
#else
.section .init
	pop %rbp
	ret

.section .fini
	pop %rbp
	ret
#endif
