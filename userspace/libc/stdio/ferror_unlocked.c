// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:53 BRT
// Last edited on January 05 of 2020, at 13:00 BRT

#define __STDIO__
#include <stdio.h>

int ferror_unlocked(FILE *stream) {
	if (stream == NULL) {									// Check if the stream is valid
		return 0;
	}
	
	return (stream->flags & __FLAGS_ERROR) ? EOF : 0;		// Check if the flag is set, and return EOF in case it is...
}
