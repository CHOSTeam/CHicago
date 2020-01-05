// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:20 BRT
// Last edited on January 03 of 2020, at 10:26 BRT

#include <stddef.h>

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {													// Now compare everything... but just like strncat, we should only compare 'n' characters...
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		
		s1++;
		s2++;
	}
	
	return 0;
}
