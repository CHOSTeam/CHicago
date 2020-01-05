// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:21 BRT
// Last edited on January 05 of 2020, at 16:22 BRT

char *strcpy(char *restrict s1, const char *restrict s2) {
	for (char *d = s1; (*d++ = *s2++);) ;			// We should optimize this later...
	return s1;
}
