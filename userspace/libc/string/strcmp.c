// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:20 BRT
// Last edited on January 03 of 2020, at 10:26 BRT

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {						// And let's compare everything...
		s1++;
		s2++;
	}
	
	return *s1 - *s2;									// Finally, return if we have any different byte...
}
