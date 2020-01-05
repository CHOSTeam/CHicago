// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:15 BRT
// Last edited on January 05 of 2020, at 16:11 BRT

#include <stddef.h>

void *memset(void *s, int c, size_t n) {
	for (char *buf = s; n; n--) {				// TODO: Optimize this...
		*buf++ = c;
	}
	
	return  s;
}
