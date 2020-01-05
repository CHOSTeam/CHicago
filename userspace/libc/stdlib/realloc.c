// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 11:02 BRT
// Last edited on January 03 of 2020, at 11:06 BRT

#include <chicago/alloc.h>

#include <stdlib.h>

void *realloc(void *ptr, size_t size) {
	if (ptr == NULL) {
		return malloc(size);								// Redirect to malloc
	}
	
	return (void*)MmReallocMemory((UIntPtr)ptr, size);		// Redirect to MmReallocMemory
}
