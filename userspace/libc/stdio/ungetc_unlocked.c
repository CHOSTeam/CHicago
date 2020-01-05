// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 21:17 BRT
// Last edited on January 04 of 2020, at 22:16 BRT

#define __STDIO__
#include <stdio.h>

int ungetc_unlocked(int c, FILE *stream) {
	if (stream == NULL || c == EOF || stream->unget != EOF) {							// Make sure that the stream isn't a NULL pointer, after that, we should fail in case the character is EOF and in case the user already used ungetc in this stream before
		return EOF;
	} else if (!(stream->flags & __FLAGS_READ)) {										// Make sure that we can read this file
		return EOF;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_WRITING)) {		// Make sure that we hadn't any error/eof, and that we haven't called any write function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_READING;													// Set that we are reading!
	stream->flags &= ~__FLAGS_EOF;														// Unset the EOF flag
	stream->unget = c;																	// Set the unget character
	stream->pos--;																		// Decrease the position
	
	return c;
}
