// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:13 BRT
// Last edited on January 05 of 2020, at 13:42 BRT

#include <chicago/file.h>

#define __STDIO__
#include <stdio.h>
#include <string.h>

int fputs_unlocked(const char *restrict s, FILE *restrict stream) {
	if (stream == NULL) {																					// Make sure that the stream isn't a NULL pointer
		return EOF;
	} else if (!(stream->flags & __FLAGS_WRITE)) {															// Make sure that we can write to this file
		return EOF;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_READING)) {							// Make sure that we hadn't any error/eof, and that we haven't called any read function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_WRITING;																		// Set that we are writing!
	
	if (stream->flags & _IONBF) {																			// Should we use buffering?
		size_t len = strlen(s);																				// Nope, get the string length
		UIntPtr written = FsWriteFile(stream->file, len, (PUInt8)s);										// Write it
		
		if (written < len) {
			stream->flags |= __FLAGS_ERROR;																	// ...
			return EOF;	
		}
		
		stream->pos += len;																					// Increase the position
		
		return (int)len;
	}
	
	int num = 0;
	
	while (*s) {																							// And here we go!
		stream->buf[stream->buf_pos++] = *s;																// Write the character
		stream->pos++;																						// Increase the position
		
		if (stream->buf_pos >= stream->buf_size ||
			((stream->flags & _IOLBF) && *s == '\n')) {														// Should we flush the buffer?
			if (FsWriteFile(stream->file, stream->buf_pos, (PUInt8)stream->buf) != stream->buf_pos) {		// Yes, do it!
				stream->flags |= __FLAGS_ERROR;																// Failed, set the error flag
				return EOF;
			}
			
			stream->buf_pos = 0;																			// Reset the position
		}
		
		s++;
		num++;
	}
	
	return num;
}
