// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 23:03 BRT
// Last edited on January 03 of 2020, at 23:23 BRT

#include <stdio.h>

int printf_unlocked(const char *restrict format, ...) {
	va_list arg;															// Let's init our arg list
	va_start(arg, format);
	return vfprintf_unlocked(stdout, format, arg);							// Call vfprintf with stdout as the first arg
}
