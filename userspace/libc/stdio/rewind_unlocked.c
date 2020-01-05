// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 12:48 BRT
// Last edited on January 05 of 2020, at 12:51 BRT

#include <stdio.h>

void rewind_unlocked(FILE *stream) {
	if (stream == NULL) {						// Check if the stream is valid
		return;
	}
	
	fseek_unlocked(stream, 0, SEEK_SET);		// Go to the start of the file
	clearerr_unlocked(stream);					// Clear any set error flag
}
