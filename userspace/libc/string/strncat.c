// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:18 BRT
// Last edited on January 03 of 2020, at 11:11 BRT

#include <stddef.h>

char *strncat(char *restrict s1, const char *restrict s2, size_t n) {
	char *s = s1;
	
	while (*s) {											// Go to the end of the destination string
		s++;
	}
	
	while (n--) {											// And let's copy, but we can only copy 'n' characters
		if (!(*s++ = *s2++)) {								// End of the string?
			return s1;										// Yes :)
		}
	}
	
	*s = 0;													// End it and return...
	
	return s1;
}
