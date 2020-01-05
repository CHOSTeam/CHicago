// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 19:16 BRT
// Last edited on January 05 of 2020, at 17:30 BRT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int __fopen_parse_mode(const char *mode);

FILE *fopen(const char *filename, const char *mode) {
	if (filename == NULL || mode == NULL) {								// First, make sure that the filename nor the mode is a NULL pointer
		return NULL;
	}
	
	int pmode = __fopen_parse_mode(mode);								// Parse the mode
	__platform_fname_t fname = __get_fname(filename);					// Convert the filename (if we need to)
	
	if (fname == NULL) {
		return NULL;													// Failed... (TODO: Set errno)
	}
	
	__platform_file_t file = __open(fname, pmode);						// Open the file
	
	if (file == -1) {
		free(fname);
		return NULL;
	}
	
	__platform_lock_t lock = __create_lock();							// Create the lock
	
	if (lock == -1) {
		__close(file);													// Failed...
		free(fname);
		return NULL;
	}
	
	FILE *ret = malloc(sizeof(FILE));									// Alloc the file struct
	
	if (ret == NULL) {
		__remove_lock(lock);											// Failed...
		__close(file);
		free(fname);
		return NULL;
	}
	
	ret->file = file;													// Setup it...
	ret->lock = lock;
	ret->filename = fname;
	ret->buf = malloc(BUFSIZ);											// Alloc the buffer
	
	if (ret->buf == NULL) {
		free(ret);														// Failed...
		__remove_lock(lock);											// Failed...
		__close(file);
		free(fname);
		return NULL;
	}
	
	ret->buf_free = 1;
	ret->buf_size = BUFSIZ;
	ret->buf_read = 0;
	ret->buf_pos = 0;
	ret->pos = 0;
	ret->unget = EOF;
	ret->flags = pmode | _IOFBF;
	
	if (!__add_stream(ret)) {											// And add it to the list!
		free(ret->buf);													// Failed...
		free(ret);
		__remove_lock(lock);
		__close(file);
		free(fname);
		return NULL;
	}
	
	return ret;
}
