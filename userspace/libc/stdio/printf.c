// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 23:01 BRT
// Last edited on January 03 of 2020, at 23:36 BRT

#include <stdio.h>

int printf(const char *restrict format, ...) {
	va_list arg;															// Let's init our arg list
	va_start(arg, format);
	int ret = vfprintf(stdout, format, arg);								// Call vfprintf with stdout as the first arg
	va_end(arg);
	return ret;
}
