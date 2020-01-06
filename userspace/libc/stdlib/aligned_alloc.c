// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 22:48 BRT
// Last edited on January 05 of 2020, at 22:49 BRT

#include <chlibc/platform.h>

void *aligned_alloc(size_t alignment, size_t size) {
	return __aalloc(alignment, size);					// Redirect...
}
