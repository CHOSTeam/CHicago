// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:17 BRT
// Last edited on January 05 of 2020, at 16:25 BRT

char *strcat(char *restrict s1, const char *restrict s2) {
	char *d = s1;						// Save the destination string start
	
	while (*d) d++ ;					// Go to the end of the destination string
	while ((*d++ = *s2++)) ;			// ... We should optimize this later...
	
	return s1;
}
