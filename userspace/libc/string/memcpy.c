// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:13 BRT
// Last edited on January 03 of 2020, at 11:10 BRT

#include <chicago/string.h>

#include <stddef.h>

void *memcpy(void *restrict s1, const void *s2, size_t n) {
	return StrCopyMemory(s1, (void*)s2, n);					// Redirect to StrCopyMemory
}
