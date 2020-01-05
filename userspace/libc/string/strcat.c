// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:17 BRT
// Last edited on January 03 of 2020, at 11:10 BRT

#include <chicago/string.h>

char *strcat(char *restrict s1, const char *restrict s2) {
	StrConcatenateC(s1, (char*)s2);				// Redirect to StrConcatenateC
	return s1;
}
