// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 22:35 BRT
// Last edited on January 04 of 2020, at 23:12 BRT

#include <chicago/file.h>
#include <chicago/list.h>
#include <chicago/misc.h>
#include <chicago/process.h>
#include <chicago/string.h>

#define __STDIO__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern List __file_list;
extern int __fopen_parse_mode(const char *mode);

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

FILE *freopen(const char *restrict filename, const char *restrict mode, FILE *restrict stream) {
	if (stream == NULL || mode == NULL) {						// First, check if all the pointers are valid
		return NULL;
	}
	
	int pmode = __fopen_parse_mode(mode);						// Parse the mode
	
	__remove_from_list(&__file_list, stream);					// Remove the stream from the list
	fflush_unlocked(stream);									// Flush the stream
	SysCloseHandle(stream->file);								// Close the handles
	SysCloseHandle(stream->lock);
	
	if (stream->buf != NULL && stream->buf_free) {				// We have a buffer that we need to free?
		free(stream->buf);										// Yes, free it
	}
	
	PWChar fname = stream->filename;							// Alright, let's get the filename
	
	if (filename != NULL) {
		free(fname);											// We don't need this saved filename
		
		size_t fnamelen = strlen(filename);						// Get the length
		fname = malloc((fnamelen + 1) * sizeof(WChar));			// Alloc space for converting the filename into unicode
		
		if (fname == NULL) {
			free(stream);
			return NULL;
		}
		
		StrUnicodeFromC(fname, (char*)filename, fnamelen);		// Convert it!
	}
	
	stream->file = FsOpenFile(fname);							// Open the file
	
	if (stream->file == -1) {
		free(fname);
		free(stream);
		return NULL;
	}
	
	stream->lock = PsCreateLock();								// Create the lock
	
	if (stream->lock == -1) {
		SysCloseHandle(stream->file);
		free(fname);
		free(stream);
		return NULL;
	}
	
	stream->filename = fname;
	stream->buf = malloc(BUFSIZ);								// Alloc the buffer
	
	if (stream->buf == NULL) {
		SysCloseHandle(stream->lock);
		SysCloseHandle(stream->file);
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
	
	if (!ListAdd(&__file_list, stream)) {						// And add it to the list!
		free(stream->buf);										// Failed...
		SysCloseHandle(stream->lock);
		SysCloseHandle(stream->file);
		free(fname);
		free(stream);
		return NULL;
	}
	
	return stream;
}
