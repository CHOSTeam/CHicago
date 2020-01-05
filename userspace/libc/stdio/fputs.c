// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:12 BRT
// Last edited on January 02 of 2020, at 12:12 BRT

#include <stdio.h>

int fputs(const char *restrict s, FILE *restrict stream) {
	flockfile(stream);								// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = fputs_unlocked(s, stream);
	funlockfile(stream);
	return ret;
}
