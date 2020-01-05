// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:21 BRT
// Last edited on January 05 of 2020, at 17:43 BRT

#include <stdio.h>

int ftrylockfile(FILE *stream) {
	if (stream == NULL) {										// Make sure that the stream isn't a NULL pointer
		return 1;
	}
	
	return !__try_lock(stream->file);							// And redirect to the internal try function
}
