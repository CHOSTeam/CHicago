// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:22 BRT
// Last edited on January 03 of 2020, at 10:28 BRT

#include <chicago/string.h>

#include <stddef.h>

size_t strlen(const char *s) {
	return StrGetLengthC((char*)s);				// Redirect to StrGetLengthC
}
