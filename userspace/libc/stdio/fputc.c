// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:16 BRT
// Last edited on January 02 of 2020, at 11:17 BRT

#include <stdio.h>

int fputc(int c, FILE *stream) {
	flockfile(stream);								// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = fputc_unlocked(c, stream);
	funlockfile(stream);
	return ret;
}
