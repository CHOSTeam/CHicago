// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 23:51 BRT
// Last edited on January 04 of 2020, at 23:51 BRT

#include <stdio.h>

int setbuf_unlocked(FILE *restrict stream, char *restrict buf) {
	return setvbuf_unlocked(stream, buf, buf == NULL ? _IONBF : _IOFBF, BUFSIZ);				// Just redirect to setvbuf...
}
