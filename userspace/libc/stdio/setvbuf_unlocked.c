// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 23:29 BRT
// Last edited on January 05 of 2020, at 13:52 BRT

#define __STDIO__
#include <stdio.h>
#include <stdlib.h>

int setvbuf_unlocked(FILE *restrict stream, char *restrict buf, int mode, size_t size) {
	if (stream == NULL || (mode != _IONBF && size == 0)) {			// Check if the stream isn't a NULL pointer and that the size is valid
		return EOF;
	}
	
	fflush_unlocked(stream);										// Flush the current buffer (if any)
	
	if (mode == _IONBF) {											// Disable buffering?
		if (stream->buf != NULL && stream->buf_free) {				// Yeah! Free the current buffer (if we need to)
			free(stream->buf);
			
		}
		
		stream->buf = NULL;											// Yeah!
		stream->buf_free = 0;
		stream->buf_size = 0;
		stream->buf_read = 0;
		stream->buf_pos = 0;
	} else if (mode == _IOFBF || mode == _IOLBF) {
		int bfree = 0;												// No, we're going to stay with buffering enabled/we're going to enable it... 
		
		if (buf == NULL) {											// Should we alloc the buffer?
			buf = malloc(size);										// Yes...
			
			if (buf == NULL) {
				return EOF;											// Failed
			}
			
			bfree = 1;
		}
		
		if (stream->buf != NULL && stream->buf_free) {				// Yeah! Free the current buffer (if we need to)
			free(stream->buf);
		}
		
		stream->buf = buf;											// Set the buffer and everything else
		stream->buf_free = bfree;
		stream->buf_size = size;
		stream->buf_read = 0;
		stream->buf_pos = 0;
	} else {
		return EOF;													// Unrecognized mode..
	}
	
	stream->flags &= ~(_IOFBF | _IOLBF | _IONBF);					// Unset any buffering flag
	stream->flags |= mode;											// And set what the user requested
	
	return 0;
}
