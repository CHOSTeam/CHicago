// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:25 BRT
// Last edited on January 05 of 2020, at 17:42 BRT

#define __STDIO__
#include <stdio.h>

int fseek_unlocked(FILE *stream, long int offset, int whence) {
	if (stream == NULL) {							// Check if the stream is valid
		return EOF;
	}
	
	fflush_unlocked(stream);						// Flush the stream
	__seek(stream->file, offset, whence);			// Set the position (in the real file)
	
	stream->flags &= ~(__FLAGS_EOF);				// Unset the EOF flag
	stream->pos = __tell(stream->file);				// And set our internal saved position
	
	return 0;
}
