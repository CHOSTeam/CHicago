// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:45 BRT
// Last edited on January 05 of 2020, at 17:54 BRT

#include <chicago/file.h>
#include <chicago/misc.h>
#include <chicago/string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__platform_fname_t __get_fname(const char *filename) {
	size_t len = strlen(filename);													// Get the filename length
	PWChar ret = malloc((len + 1) * sizeof(WChar));									// Alloc space for converting it into unicode
	
	if (ret == NULL) {
		return NULL;
	}
	
	StrUnicodeFromC(ret, (char*)filename, len);										// Convert it!
	
	return ret;
}

__platform_file_t __open(__platform_fname_t fname, int mode) {
	(void)mode;																		// We can't handle the mode yet...
	return FsOpenFile(fname);
}

void __close(__platform_file_t file) {
	SysCloseHandle(file);
}

size_t __read(__platform_file_t file, size_t size, void *buf) {
	return FsReadFile(file, size, (PUInt8)buf);
}

size_t __write(__platform_file_t file, size_t size, const void *buf) {
	return FsWriteFile(file, size, (PUInt8)buf);
}

void __seek(__platform_file_t file, long int offset, int whence) {
	FsSetPosition(file, whence, offset);
}

long int __tell(__platform_file_t file) {
	return FsGetPosition(file);
}
