// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 23:36 BRT
// Last edited on January 03 of 2020, at 23:37 BRT

#include <stdio.h>

int snprintf(char *restrict s, size_t n, const char *restrict format, ...) {
	va_list arg;															// Let's init our arg list
	va_start(arg, format);
	int ret = vsnprintf(s, n, format, arg);									// Call vsnprintf
	va_end(arg);
	return ret;
}
