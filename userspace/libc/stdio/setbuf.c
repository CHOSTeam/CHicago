// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 23:50 BRT
// Last edited on January 04 of 2020, at 23:51 BRT

#include <stdio.h>

int setbuf(FILE *restrict stream, char *restrict buf) {
	return setvbuf(stream, buf, buf == NULL ? _IONBF : _IOFBF, BUFSIZ);				// Just redirect to setvbuf...
}
