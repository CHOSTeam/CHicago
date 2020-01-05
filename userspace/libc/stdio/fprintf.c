// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 22:54 BRT
// Last edited on January 03 of 2020, at 23:36 BRT

#include <stdio.h>

int fprintf(FILE *restrict stream, const char *restrict format, ...) {
	va_list arg;															// Let's init our arg list
	va_start(arg, format);
	int ret = vfprintf(stream, format, arg);								// Call vfprintf
	va_end(arg);
	return ret;
}
