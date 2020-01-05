// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 17:07 BRT
// Last edited on January 05 of 2020, at 17:21 BRT

#include <chlibc/platform.h>

void *malloc(size_t size) {
	return __alloc(size);				// Redirect...
}

