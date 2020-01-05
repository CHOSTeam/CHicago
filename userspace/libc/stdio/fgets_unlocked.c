// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:02 BRT
// Last edited on January 05 of 2020, at 17:38 BRT

#define __STDIO__
#include <stdio.h>

char *fgets_unlocked(char *restrict s, int n, FILE *restrict stream) {
	if (stream == NULL || n == 0) {																// Null pointer check and check if the num isn't 0
		return NULL;
	} else if (!(stream->flags & __FLAGS_READ)) {												// Make sure that we can read this file
		return NULL;
	} else if (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_WRITING)) {				// Make sure that we hadn't any error/eof, and that we haven't called any write function before
		return NULL;
	}
	
	stream->flags |= __FLAGS_READING;															// Set that we are reading!
	
	if (n == 1) {																				// Just the EOS character?
		*s = '\0';																				// Yeah...
		return s;
	}
	
	char *dest = s;																				// Copy the pointer...
	int i = 0;																					// And let's go!
	
	if (stream->flags & _IONBF) {																// No buffering?
		while (++i != n) {																		// Yeah, so let's just read everything manually!
			if (stream->unget != EOF) {															// Use the unget buffer?
				*dest++ = stream->unget;														// Yeah
				stream->unget = EOF;
			} else if (__read(stream->file, 1, dest++) != 1) {
				stream->flags |= __FLAGS_EOF;													// End of file...
				return NULL;
			}
			
			stream->pos++;																		// Increase the position
			
			if (*(dest - 1) == '\n') {															// End of the string?
				break;																			// Yes!
			}
		}
		
		*dest = '\0';																			// End the string
		
		return s;
	}
	
	while (++i != n) {																			// So, we need to use buffering... here we go!
		if (stream->unget != EOF) {																// Use the unget buffer?
			*dest++ = stream->unget;															// Yeah
			stream->unget = EOF;
			goto n;
		}
		
		if (stream->buf_pos >= stream->buf_size || stream->buf_read == 0) {						// We need to fill the buffer?
			size_t read = __read(stream->file, stream->buf_size, stream->buf);					// Yes, read in

			if (read == 0) {																	// Failed?
				stream->flags |= __FLAGS_EOF;													// Yup. that's EOF...
				return NULL;
			}

			stream->buf_read = read;															// Setup and clean everything else
			stream->buf_pos = 0;
		}
		
		*dest++ = stream->buf[stream->buf_pos++];												// Read in the character
n:		stream->pos++;																			// Increase the position
		
		if (*(dest - 1) == '\n') {																// End of the string?
			break;																				// Yes!
		}
	}
	
	*dest = '\0';
	
	return s;
}

