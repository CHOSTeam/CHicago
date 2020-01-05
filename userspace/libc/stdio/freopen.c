// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 22:35 BRT
// Last edited on January 05 of 2020, at 17:43 BRT

#define __STDIO__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int __fopen_parse_mode(const char *mode);

FILE *freopen(const char *restrict filename, const char *restrict mode, FILE *restrict stream) {
	if (stream == NULL || mode == NULL) {						// First, check if all the pointers are valid
		return NULL;
	}
	
	int pmode = __fopen_parse_mode(mode);						// Parse the mode
	
	__remove_stream(stream);									// Remove the stream from the list
	fflush_unlocked(stream);									// Flush the stream
	__close(stream->file);										// Close everything that we need to close
	__remove_lock(stream->lock);
	
	if (stream->buf != NULL && stream->buf_free) {				// We have a buffer that we need to free?
		free(stream->buf);										// Yes, free it
	}
	
	__platform_fname_t fname = filename == NULL ?
							   stream->filename :
							   __get_fname(filename);			// Alright, let's get the filename
	
	if (filename != NULL) {										// Free the old filename (if we need to)
		free(stream->filename);
	}
	
	if (fname == NULL) {
		free(stream);
		return NULL;
	}
	
	stream->file = __open(fname, pmode);						// Open the file
	
	if (stream->file == -1) {
		free(fname);
		free(stream);
		return NULL;
	}
	
	stream->lock = __create_lock();								// Create the lock
	
	if (stream->lock == -1) {
		__close(stream->file);
		free(fname);
		free(stream);
		return NULL;
	}
	
	stream->filename = fname;
	stream->buf = malloc(BUFSIZ);								// Alloc the buffer
	
	if (stream->buf == NULL) {
		__remove_lock(stream->lock);
		__close(stream->file);
		free(fname);
		free(stream);
		return NULL;
	}
	
	stream->buf_free = 1;
	stream->buf_size = BUFSIZ;
	stream->buf_read = 0;
	stream->buf_pos = 0;
	stream->pos = 0;
	stream->unget = EOF;
	stream->flags = pmode | _IOFBF;
	
	if (!__add_stream(stream)) {								// And add it to the list!
		free(stream->buf);										// Failed...
		__remove_lock(stream->lock);
		__close(stream->file);
		free(fname);
		free(stream);
		return NULL;
	}
	
	return stream;
}
