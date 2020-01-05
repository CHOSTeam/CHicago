// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:48 BRT
// Last edited on January 05 of 2020, at 12:49 BRT

#include <stdio.h>

void rewind(FILE *stream) {
	flockfile(stream);									// Lock the file, call the unlocked version of this function, and in the end unlock the file
	rewind_unlocked(stream);
	funlockfile(stream);
}
