// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:41 BRT
// Last edited on January 05 of 2020, at 12:42 BRT

#include <stdio.h>

long int ftell(FILE *stream) {
	flockfile(stream);									// Lock the file, call the unlocked version of this function, and in the end unlock the file
	long int ret = ftell_unlocked(stream);
	funlockfile(stream);
	return ret;
}
