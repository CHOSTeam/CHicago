// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 11:01 BRT
// Last edited on January 05 of 2020, at 17:38 BRT

#define __STDIO__
#include <stdio.h>
#include <string.h>

size_t fread_unlocked(void *restrict ptr, size_t size, size_t count, FILE *restrict stream) {
	if (stream == NULL || size == 0 || count == 0) {												// First, sanity checks
		return 0;
	} else if (!(stream->flags & __FLAGS_READ)) {													// Make sure that we can read this file
		return EOF;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_WRITING)) {					// Make sure that we hadn't any error/eof, and that we haven't called any read function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_READING;																// Set that we are reading!
	
	char *dest = ptr;																				// Cast the pointer
	
	if (stream->flags & _IONBF) {																	// No buffering?
		for (size_t i = 0; i < count; i++) {														// Yeah, so let's just read all the items!
			if (__read(stream->file, size, dest) != size) {
				stream->flags |= __FLAGS_EOF;														// End of file...
				return i;
			}
			
			dest += size;
			stream->pos += size;
		}
		
		goto e;
	}
	
	for (size_t i = 0; i < count; i++) {															// We need to use buffering... here we go!
		for (size_t j = 0; j < size; j++) {
			if (stream->unget != EOF) {																// Use the unget buffer?
				*dest++ = stream->unget;															// Yeah
				stream->unget = EOF;
				stream->pos++;																		// Increase the position
				continue;
			}
			
			if (stream->buf_pos >= stream->buf_size || stream->buf_read == 0) {						// We need to fill the buffer?
				size_t read = __read(stream->file, stream->buf_size, stream->buf);					// Yes, read in

				if (read == 0) {																	// Failed?
					stream->flags |= __FLAGS_EOF;													// Yup. that's EOF...
					return i;
				}

				stream->buf_read = read;															// Setup and clean everything else
				stream->buf_pos = 0;
			}
			
			*dest++ = stream->buf[stream->buf_pos];													// Read in from the buffer...
			stream->pos++;																			// Increase the position
		}
	}
	
e:	return count;
}
