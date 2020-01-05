// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:19 BRT
// Last edited on January 02 of 2020, at 11:20 BRT

#include <chicago/process.h>

#include <stdio.h>

void flockfile(FILE *stream) {
	if (stream != NULL) {										// Make sure that the stream isn't a NULL pointer
		PsLock(stream->lock);									// And redirect to the PsLock function...
	}
}
