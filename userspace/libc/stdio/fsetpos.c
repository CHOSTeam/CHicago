// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:20 BRT
// Last edited on January 05 of 2020, at 12:34 BRT

#include <stdio.h>

int fsetpos(FILE *stream, const fpos_t *pos) {
	return fseek(stream, *pos, SEEK_SET);			// For now we can just redirect to fseek
}
