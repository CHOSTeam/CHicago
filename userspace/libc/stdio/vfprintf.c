// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 22:47 BRT
// Last edited on January 03 of 2020, at 22:48 BRT

#include <stdio.h>

int vfprintf(FILE *restrict stream, const char *restrict format, va_list arg) {
	flockfile(stream);											// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = vfprintf_unlocked(stream, format, arg);
	funlockfile(stream);
	return ret;
}
