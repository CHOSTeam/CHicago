// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:09 BRT
// Last edited on January 02 of 2020, at 12:09 BRT

#include <stdio.h>

char *fgets(char *restrict s, int n, FILE *restrict stream) {
	flockfile(stream);								// Lock the file, call the unlocked version of this function, and in the end unlock the file
	char *ret = fgets_unlocked(s, n, stream);
	funlockfile(stream);
	return ret;
}
