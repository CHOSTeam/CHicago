// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 22:20 BRT
// Last edited on January 04 of 2020, at 22:31 BRT

#include <chicago/file.h>
#include <chicago/list.h>

#define __STDIO__
#include <stdio.h>

extern List __file_list;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

static int __flush(FILE *stream) {
	if (stream->flags & __FLAGS_READING) {																	// Are we reading at the moment?
		if (stream->unget != EOF) {																			// Yeah, we need to clear the unget buffer?
			stream->unget = EOF;																			// Yes, clear it and fix the position
			stream->pos++;
		}
		
		FsSetPosition(stream->file, 0, stream->pos - (stream->buf_read - stream->buf_pos));					// Set the file position
		
		stream->buf_read = 0;																				// Clear those saved buffering positions
		stream->buf_pos = 0;
		stream->flags &= ~(stream->flags & __FLAGS_READING);												// And unset the reading flag
	} else if (stream->flags & __FLAGS_WRITING) {															// Or writing?
		if (stream->buf_pos != 0) {																			// Yeah, we have something to flush?
			if (FsWriteFile(stream->file, stream->buf_pos, (PUInt8)stream->buf) != stream->buf_pos) {		// Yes, let's flush it
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
		
		ListForeach(&__file_list, i) {																		// Now flush everything else (all the other open streams)
			if (__flush(i->data) == EOF) {
				return EOF;
			}
		}
		
		return 0;
	}
	
	return __flush(stream);																					// Just redirect to __flush
}
