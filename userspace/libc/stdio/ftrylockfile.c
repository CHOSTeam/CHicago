// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:21 BRT
// Last edited on January 02 of 2020, at 11:27 BRT

#include <chicago/process.h>

#include <stdio.h>

int ftrylockfile(FILE *stream) {
	if (stream == NULL) {										// Make sure that the stream isn't a NULL pointer
		return 1;
	}
	
	return !PsTryLock(stream->lock);							// And redirect to the PsTryLock function
}
