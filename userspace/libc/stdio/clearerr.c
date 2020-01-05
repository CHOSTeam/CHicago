// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:52 BRT
// Last edited on January 05 of 2020, at 12:56 BRT

#include <stdio.h>

void clearerr(FILE *stream) {
	flockfile(stream);									// Lock the file, call the unlocked version of this function, and in the end unlock the file
	clearerr_unlocked(stream);
	funlockfile(stream);
}
