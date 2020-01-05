// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 22:22 BRT
// Last edited on January 04 of 2020, at 22:30 BRT

#include <chicago/list.h>

#include <stdio.h>

extern List __file_list;

int fflush(FILE *stream) {
	if (stream == NULL) {													// Should we just flush everything?
		if (stdin != NULL && fflush(stdin) == EOF) {						// Yeah, start with the stdstreams
			return EOF;
		} else if (stdout != NULL && fflush(stdout) == EOF) {
			return EOF;
		} else if (stderr != NULL && fflush(stderr) == EOF) {
			return EOF;
		}
		
		ListForeach(&__file_list, i) {										// Now flush everything else (all the other open streams)
			if (fflush(i->data) == EOF) {
				return EOF;
			}
		}
		
		return 0;
	}
	
	flockfile(stream);														// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = fflush_unlocked(stream);
	funlockfile(stream);
	
	return ret;
}
