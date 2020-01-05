// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:14 BRT
// Last edited on January 03 of 2020, at 10:28 BRT

#include <string.h>

void *memmove(void *s1, const void *s2, size_t n) {
	if (!(s2 < s1)) {											// Both pointers overlap?
		return memcpy(s1, s2, n);								// No, so let's just redirect to memcpy
	}
	
	char *d = (char*)s1;										// Cast the void pointers into char pointers
	const char *s = (const char*)s2;
	
	for (d += n, s += n; ; n--) {								// Yes, so let's copy backwards...
		*d-- = *s--;
	}
	
	return (void*)s1;
}
