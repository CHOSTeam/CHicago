// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 23:13 BRT
// Last edited on January 04 of 2020, at 23:26 BRT

#include <chicago/list.h>
#include <chicago/misc.h>

#include <stdio.h>
#include <stdlib.h>

extern List __file_list;

static void __remove_from_list(PList list, PVoid item) {
	UIntPtr idx = 0;											// Let's find the item in the list
	int found = 0;
	
	ListForeach(list, i) {
		if (i->data == item) {
			found = 1;											// FOUND!
			break;
		}
		
		idx++;
	}
	
	if (found) {
		ListRemove(list, idx);									// Remove it!
	}
}

int fclose(FILE *stream) {
	if (stream == NULL) {										// Make sure that the stream is valid
		return EOF;
	} else if (fflush_unlocked(stream) == EOF) {				// Flush the stream
		return EOF;
	}
	
	__remove_from_list(&__file_list, stream);					// Remove the stream from the list
	SysCloseHandle(stream->file);								// Close the handles
	SysCloseHandle(stream->lock);
	
	if (stream->buf != NULL && stream->buf_free) {				// We have a buffer that we need to free?
		free(stream->buf);										// Yes, free it
	}
	
	free(stream->filename);										// Free the filename
	free(stream);												// Free the stream struct
	
	return 0;
}
