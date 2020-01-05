// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:13 BRT
// Last edited on January 05 of 2020, at 16:09 BRT

#include <stddef.h>

void *memcpy(void *restrict s1, const void *s2, size_t n) {
	for (char *d = s1, *s = (char*)s2; n; n--) {			// TODO: Optimize this...
		*d++ = *s++;
	}
	
	return s1;
}
