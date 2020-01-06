// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:55 BRT
// Last edited on January 05 of 2020, at 22:50 BRT

#include <chicago/alloc.h>

#include <stddef.h>

void *__alloc(size_t size) {
	return (void*)MmAllocMemory(size);
}

void *__aalloc(size_t align, size_t size) {
	return (void*)MmAllocAlignedMemory(size, align);
}

void __free(void *ptr) {
	MmFreeMemory((UIntPtr)ptr);
}

void *__realloc(void *ptr, size_t size) {
	return (void*)MmReallocMemory((UIntPtr)ptr, size);
}
