// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 21:23 BRT
// Last edited on January 05 of 2020, at 17:38 BRT

#define __STDIO__
#include <stdio.h>

int fgetc_unlocked(FILE *stream) {
	if (stream == NULL) {																	// Make sure that the stream isn't a NULL pointer
		return EOF;
	} else if (!(stream->flags & __FLAGS_READ)) {											// Make sure that we can read this file
		return EOF;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_WRITING)) {			// Make sure that we hadn't any error/eof, and that we haven't called any write function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_READING;														// Set that we are reading!
	
	if (stream->unget != EOF) {																// The user called ungetc before?
		int ret = stream->unget;															// Yes, save the unget content
		stream->unget = EOF;																// Set the unget content to EOF (no content)
		stream->pos++;																		// Increase the position
		return ret;																			// And return the saved value
	} else if (stream->flags & _IONBF) {													// No buffering?
		char ret;																			// Yeah! Let's call FsReadFile directly and only read one byte!
		
		if (__read(stream->file, 1, &ret) != 1) {
			stream->flags |= __FLAGS_EOF;													// End of file, set the EOF flag and return
			return EOF;
		}
		
		stream->pos++;																		// Increase the position
		
		return ret;
	} else if (stream->buf_pos >= stream->buf_size || stream->buf_read == 0) {				// We need to fill the buffer?
		size_t read = __read(stream->file, stream->buf_size, stream->buf);					// Yes, read in
		
		if (read == 0) {																	// Failed?
			stream->flags |= __FLAGS_EOF;													// Yup. that's EOF...
			return EOF;
		}
		
		stream->buf_read = read;															// Setup and clean everything else
		stream->buf_pos = 0;
	}
	
	stream->pos++;																			// Increase the position
	
	return stream->buf[stream->buf_pos++];
}
