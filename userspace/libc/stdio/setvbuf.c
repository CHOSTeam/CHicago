// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 23:29 BRT
// Last edited on January 04 of 2020, at 23:48 BRT

#include <stdio.h>

int setvbuf(FILE *restrict stream, char *restrict buf, int mode, size_t size) {
	flockfile(stream);												// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = setvbuf_unlocked(stream, buf, mode, size);
	funlockfile(stream);
	return ret;
}
