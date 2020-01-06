// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 22:42 BRT
// Last edited on January 05 of 2020, at 22:44 BRT

#include <chlibc/platform.h>

void *__alloc_virt(size_t size) {
	return (void*)VirtAllocAddress(0, size, VIRT_PROT_READ | VIRT_PROT_WRITE);
}

void __free_virt(void *addr, size_t size) {
	VirtFreeAddress((UIntPtr)addr, size);
}
