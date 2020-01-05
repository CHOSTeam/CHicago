// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:22 BRT
// Last edited on January 03 of 2020, at 11:10 BRT

#include <stddef.h>

char *strncpy(char *restrict s1, const char *restrict s2, size_t n) {
	char *s = s1;
	
	while (*s2 && n--) {											// Copy until we find the end of the source string (or until we have copied 'n' characters)
		*s++ = *s2++;
	}
	
	while (n--) {													// Pad with zeroes
		*s++ = '\0';
	}
	
	return s1;														// And return
}
