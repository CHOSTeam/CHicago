// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 22:59 BRT
// Last edited on January 03 of 2020, at 22:59 BRT

#include <stdio.h>

int vprintf_unlocked(const char *restrict format, va_list arg) {
	return vfprintf_unlocked(stdout, format, arg);							// Redirect to vfprintf with stdout as the first arg
}
