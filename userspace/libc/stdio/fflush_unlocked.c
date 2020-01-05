// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 22:20 BRT
// Last edited on January 05 of 2020, at 17:42 BRT

#define __STDIO__
#include <stdio.h>

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

static int __flush(FILE *stream) {
	if (stream->flags & __FLAGS_READING) {																	// Are we reading at the moment?
		if (stream->unget != EOF) {																			// Yeah, we need to clear the unget buffer?
			stream->unget = EOF;																			// Yes, clear it and fix the position
			stream->pos++;
		}
		
		__seek(stream->file, stream->pos - (stream->buf_read - stream->buf_pos), SEEK_SET);					// Set the file position
		
		stream->buf_read = 0;																				// Clear those saved buffering positions
		stream->buf_pos = 0;
		stream->flags &= ~(stream->flags & __FLAGS_READING);												// And unset the reading flag
	} else if (stream->flags & __FLAGS_WRITING) {															// Or writing?
		if (stream->buf_pos != 0) {																			// Yeah, we have something to flush?
			if (__write(stream->file, stream->buf_pos, stream->buf) != stream->buf_pos) {					// Yes, let's flush it
				stream->flags |= __FLAGS_ERROR;																// ... Failed...
				return EOF;
			}
			
			stream->buf_pos = 0;																			// And set the position to zero
		}
		
		stream->flags &= ~(stream->flags & __FLAGS_WRITING);												// And clear the writing flag
	}
	
	return 0;
}

int fflush_unlocked(FILE *stream) {
	if (stream == NULL) {																					// Should we just flush everything?
		if (stdin != NULL && __flush(stdin) == EOF) {														// Yeah, start with the stdstreams
			return EOF;
		} else if (stdout != NULL && __flush(stdout) == EOF) {
			return EOF;
		} else if (stderr != NULL && __flush(stderr) == EOF) {
			return EOF;
		}
		
		void *last = __get_stream_list();																	// Get the start of the stream list
		
		while (last != NULL) {																				// And here we go...
			if (__flush(__get_stream(last)) == EOF) {														// Flush
				return EOF;
			}
			
			last = __get_next_stream(last);																	// Get the next stream
		}
		
		return 0;
	}
	
	return __flush(stream);																					// Just redirect to __flush
}
