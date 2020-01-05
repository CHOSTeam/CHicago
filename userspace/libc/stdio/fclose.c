// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 23:13 BRT
// Last edited on January 05 of 2020, at 17:15 BRT

#include <stdio.h>
#include <stdlib.h>

int fclose(FILE *stream) {
	if (stream == NULL) {										// Make sure that the stream is valid
		return EOF;
	} else if (fflush_unlocked(stream) == EOF) {				// Flush the stream
		return EOF;
	}
	
	__remove_stream(stream);									// Remove the stream from the internal list
	__close(stream->file);										// Close everything that we need to close
	__remove_lock(stream->lock);
	
	if (stream->buf != NULL && stream->buf_free) {				// We have a buffer that we need to free?
		free(stream->buf);										// Yes, free it
	}
	
	free(stream->filename);										// Free the filename
	free(stream);												// Free the stream struct
	
	return 0;
}
