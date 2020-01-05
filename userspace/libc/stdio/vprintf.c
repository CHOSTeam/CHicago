// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 22:57 BRT
// Last edited on January 03 of 2020, at 23:00 BRT

#include <stdio.h>

int vprintf(const char *restrict format, va_list arg) {
	return vfprintf(stdout, format, arg);						// Redirect to vfprintf with stdout as the first arg
}
