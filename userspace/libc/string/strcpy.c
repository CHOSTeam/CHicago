// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:21 BRT
// Last edited on January 03 of 2020, at 11:10 BRT

#include <chicago/string.h>

char *strcpy(char *restrict s1, const char *restrict s2) {
	return StrCopyC(s1, (char*)s2);				// Redirect to StrCopyC
}
