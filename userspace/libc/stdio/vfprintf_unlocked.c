// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 22:44 BRT
// Last edited on January 04 of 2020, at 21:13 BRT

#include <stdio.h>

extern int __vprint(int usestream, FILE *restrict stream, int limitn, char *restrict s, size_t n, const char *restrict format, va_list arg);

int vfprintf_unlocked(FILE *restrict stream, const char *restrict format, va_list arg) {
	return __vprint(1, stream, 0, NULL, 0, format, arg);											// Redirect to our internal print function
}
