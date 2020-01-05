// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:18 BRT
// Last edited on January 03 of 2020, at 10:48 BRT

#include <stdio.h>

size_t fwrite(const void *restrict ptr, size_t size, size_t count, FILE *restrict stream) {
	flockfile(stream);											// Lock the file, call the unlocked version of this function, and in the end unlock the file
	size_t ret = fwrite_unlocked(ptr, size, count, stream);
	funlockfile(stream);
	return ret;
}
