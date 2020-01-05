// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:15 BRT
// Last edited on January 03 of 2020, at 10:26 BRT

#include <chicago/string.h>

#include <stddef.h>

void *memset(void *s, int c, size_t n) {
	return StrSetMemory(s, c, n);				// Redirect to StrSetMemory
}
