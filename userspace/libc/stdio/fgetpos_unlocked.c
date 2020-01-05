// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:01 BRT
// Last edited on January 05 of 2020, at 12:47 BRT

#include <stdio.h>

int fgetpos_unlocked(FILE *restrict stream, fpos_t *restrict pos) {
	long int ret = ftell_unlocked(stream);						// For now we can use ftell...
	
	if (ret == EOF) {
		return EOF;
	}
	
	*pos = ret;
	
	return 0;
}
