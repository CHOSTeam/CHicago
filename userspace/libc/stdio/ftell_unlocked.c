// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:42 BRT
// Last edited on January 05 of 2020, at 12:44 BRT

#include <stdio.h>

long int ftell_unlocked(FILE *stream) {
	return stream != NULL ? stream->pos : EOF;				// Check if the stream is a valid pointer, in case it is return the current position
}
