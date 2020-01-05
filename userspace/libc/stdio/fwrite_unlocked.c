// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 10:47 BRT
// Last edited on January 05 of 2020, at 17:39 BRT

#define __STDIO__
#include <stdio.h>
#include <string.h>

size_t fwrite_unlocked(const void *restrict ptr, size_t size, size_t count, FILE *restrict stream) {
	if (stream == NULL || size == 0 || count == 0) {																// First, sanity checks
		return 0;
	} else if (!(stream->flags & __FLAGS_WRITE)) {																	// Make sure that we can write to this file
		return EOF;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_READING)) {									// Make sure that we hadn't any error/eof, and that we haven't called any read function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_WRITING;																				// Set that we are writing!
	
	const char *buf = ptr;																							// Cast the pointer
	
	if (stream->flags & _IONBF) {																					// Should we use the buffer?
		for (size_t i = 0; i < count; i++) {																		// Nope, just write each item
			if (__write(stream->file, size, buf) != size) {
				stream->flags |= __FLAGS_ERROR;																		// Failed, return error...
				return i;
			}
			
			buf += size;																							// Go to the next item
			stream->pos += size;
		}
		
		goto e;																										// Jump to the end!
	}
	
	for (size_t i = 0; i < size; i++) {																				// Ok, we should use buffering, so here we go!
		for (size_t j = 0; j < count; j++) {
			stream->buf[stream->buf_pos++] = buf[i * size + j];														// Write the current byte into the buffer
			stream->pos++;																							// Increase the position
			
			if (stream->buf_pos >= stream->buf_size ||
				((stream->flags & _IOLBF) && buf[i * size + j] == '\n')) {											// Should we flush the buffer?
				if (__write(stream->file, stream->buf_pos, stream->buf) != stream->buf_pos) {						// Yes, do it!
					stream->flags |= __FLAGS_ERROR;																	// Failed, set the error flag
					return i;
				}

				stream->buf_pos = 0;																				// Reset the position
			}
		}
	}
	
e:	return count;
}
