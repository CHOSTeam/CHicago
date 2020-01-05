// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:25 BRT
// Last edited on January 05 of 2020, at 12:32 BRT

#include <stdio.h>

int fseek(FILE *stream, long int offset, int whence) {
	flockfile(stream);									// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = fseek_unlocked(stream, offset, whence);
	funlockfile(stream);
	return ret;
}
