// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:52 BRT
// Last edited on January 05 of 2020, at 13:00 BRT

#define __STDIO__
#include <stdio.h>

void clearerr_unlocked(FILE *stream) {
	if (stream != NULL) {											// The stream is valid?
		stream->flags &= ~(__FLAGS_EOF | __FLAGS_ERROR);			// Yeah, clear the EOF and the error flag
	}
}
