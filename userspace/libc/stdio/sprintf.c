// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 23:37 BRT
// Last edited on January 03 of 2020, at 23:39 BRT

#include <stdio.h>

int sprintf(char *restrict s, const char *restrict format, ...) {
	va_list arg;															// Let's init our arg list
	va_start(arg, format);
	int ret = vsprintf(s, format, arg);										// Call vsprintf
	va_end(arg);
	return ret;
}
