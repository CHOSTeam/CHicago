// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 21:39 BRT
// Last edited on January 05 of 2020, at 13:41 BRT

#include <chicago/file.h>

#define __STDIO__
#include <stdio.h>

int fputc_unlocked(int c, FILE *stream) {
	if (stream == NULL) {																				// Make sure that the stream isn't a NULL pointer
		return EOF;
	} else if (!(stream->flags & __FLAGS_WRITE)) {														// Make sure that we can read to this file
		return EOF;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_READING)) {						// Make sure that we hadn't any error/eof, and that we haven't called any read function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_WRITING;																	// Set that we are writing!
	
	UInt8 buf = (UInt8)c;																				// Convert the data to be written
	
	if (stream->flags & _IONBF) {																		// Are we buffered?
		if (!FsWriteFile(stream->file, 1, &buf)) {														// Nope, just write!
			stream->flags |= __FLAGS_ERROR;																// Failed, set the error flag
			return EOF;
		}
		
		stream->pos++;																					// Increase the position
		
		return c;
	}
	
	stream->buf[stream->buf_pos++] = buf;																// Write the character into the buffer
	stream->pos++;																						// Increase the position
	
	if (stream->buf_pos >= stream->buf_size ||
		((stream->flags & _IOLBF) && c == '\n')) {														// Should we flush the buffer?
		if (FsWriteFile(stream->file, stream->buf_pos, (PUInt8)stream->buf) != stream->buf_pos) {		// Yes, do it!
			stream->flags |= __FLAGS_ERROR;																// Failed, set the error flag
			return EOF;
		}
		
		stream->buf_pos = 0;																			// Reset the position
	}
	
	return c;
}
