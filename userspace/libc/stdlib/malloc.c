// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 17:07 BRT
// Last edited on January 01 of 2020, at 17:07 BRT

#include <chicago/alloc.h>

#include <stddef.h>

void *malloc(size_t size) {
	return (void*)MmAllocMemory(size);
}

