// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:19 BRT
// Last edited on January 05 of 2020, at 17:42 BRT

#include <stdio.h>

void flockfile(FILE *stream) {
	if (stream != NULL) {										// Make sure that the stream isn't a NULL pointer
		__lock(stream->file);									// And redirect to the internal lock function...
	}
}
