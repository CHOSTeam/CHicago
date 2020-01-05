// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 11:02 BRT
// Last edited on January 05 of 2020, at 17:22 BRT

#include <chlibc/platform.h>

#include <stdlib.h>

void *realloc(void *ptr, size_t size) {
	if (ptr == NULL) {
		return malloc(size);								// Redirect to malloc
	}
	
	return __realloc(ptr, size);							// Redirect to the internal realloc function...
}
