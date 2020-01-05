// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 19:16 BRT
// Last edited on January 04 of 2020, at 22:53 BRT

#include <chicago/file.h>
#include <chicago/list.h>
#include <chicago/misc.h>
#include <chicago/process.h>
#include <chicago/string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int __fopen_parse_mode(const char *mode);

List __file_list = { 0 };

FILE *fopen(const char *filename, const char *mode) {
	if (filename == NULL || mode == NULL) {								// First, make sure that the filename nor the mode is a NULL pointer
		return NULL;
	}
	
	int pmode = __fopen_parse_mode(mode);								// Parse the mode
	size_t fnamelen = strlen(filename);									// Get the filename length
	PWChar fname = malloc((fnamelen + 1) * sizeof(WChar));				// Alloc space for converting the filename into unicode
	
	if (fname == NULL) {
		return NULL;													// Failed... (TODO: Set errno)
	}
	
	StrUnicodeFromC(fname, (char*)filename, fnamelen);					// Convert it!
	
	IntPtr fhndl = FsOpenFile(fname);									// Open the file
	
	if (fhndl == -1) {
		free(fname);
		return NULL;
	}
	
	IntPtr lhndl = PsCreateLock();										// Create the lock
	
	if (lhndl == -1) {
		SysCloseHandle(fhndl);											// Failed...
		free(fname);
		return NULL;
	}
	
	FILE *ret = malloc(sizeof(FILE));									// Alloc the file struct
	
	if (ret == NULL) {
		SysCloseHandle(lhndl);											// Failed...
		SysCloseHandle(fhndl);
		free(fname);
		return NULL;
	}
	
	ret->file = fhndl;													// Setup it...
	ret->lock = lhndl;
	ret->filename = fname;
	ret->buf = malloc(BUFSIZ);											// Alloc the buffer
	
	if (ret->buf == NULL) {
		free(ret);														// Failed...
		SysCloseHandle(lhndl);
		SysCloseHandle(fhndl);
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
	
	if (!ListAdd(&__file_list, ret)) {									// And add it to the list!
		free(ret->buf);													// Failed...
		free(ret);
		SysCloseHandle(lhndl);
		SysCloseHandle(fhndl);
		free(fname);
		return NULL;
	}
	
	return ret;
}
