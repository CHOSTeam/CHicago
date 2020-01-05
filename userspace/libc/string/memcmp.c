// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:12 BRT
// Last edited on January 03 of 2020, at 10:25 BRT

#include <stddef.h>

int memcmp(const void *s1, const void *s2, size_t n) {
	const char *d = (const char*)s1;							// Cast the void pointers into char pointers
	const char *s = (const char*)s2;
	
	while (n--) {												// And let's go!
		if (*d != *s) {											// Is both bytes the same?
			return *d - *s;										// No, return the difference...
		}
		
		s++;
		d++;
	}
	
	return 0;
}
