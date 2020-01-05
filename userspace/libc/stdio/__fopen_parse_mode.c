// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 19:50 BRT
// Last edited on January 04 of 2020, at 21:20 BRT

#define __STDIO__
#include <stdio.h>
#include <string.h>

int __fopen_parse_mode(const char *mode) {
	if (!strcmp(mode, "r") || !strcmp(mode, "rb")) {											// Read
		return __FLAGS_READ;
	} else if (!strcmp(mode, "w") || !strcmp(mode, "wb") ||
			   !strcmp(mode, "wx") || !strcmp(mode, "wbx")) {									// Write
		return __FLAGS_WRITE;
	} else if (!strcmp(mode, "a") || !strcmp(mode, "ab")) {										// Append
		return __FLAGS_APPEND | __FLAGS_WRITE;
	} else if (!strcmp(mode, "r+") || !strcmp(mode, "rb+") || !strcmp(mode, "r+b") ||
			   !strcmp(mode, "w+") || !strcmp(mode, "wb+") || !strcmp(mode, "w+b")) {			// Update
		return __FLAGS_READ | __FLAGS_WRITE | __FLAGS_UPDATE;
	} else if (!strcmp(mode, "w+x") || !strcmp(mode, "w+bx") || !strcmp(mode, "wb+x")) {		// Update (but fail if the file already exists)
		return __FLAGS_READ | __FLAGS_WRITE | __FLAGS_UPDATE | __FLAGS_EXCLUSIVE;
	} else if (!strcmp(mode, "a+") || !strcmp(mode, "a+b") || !strcmp(mode, "ab+")) {			// Append + Update
		return __FLAGS_APPEND | __FLAGS_WRITE | __FLAGS_UPDATE;
	}
	
	return __FLAGS_READ;																		// Return the default...
}
