// File author is Ítalo Lima Marconato Matias
//
// Created on January 04 of 2020, at 22:22 BRT
// Last edited on January 05 of 2020, at 17:03 BRT

#include <stdio.h>

int fflush(FILE *stream) {
	if (stream == NULL) {													// Should we just flush everything?
		if (stdin != NULL && fflush(stdin) == EOF) {						// Yeah, start with the stdstreams
			return EOF;
		} else if (stdout != NULL && fflush(stdout) == EOF) {
			return EOF;
		} else if (stderr != NULL && fflush(stderr) == EOF) {
			return EOF;
		}
		
		void *last = __get_stream_list();									// Get the start of the stream list
		
		while (last != NULL) {												// And here we go...
			if (fflush(__get_stream(last)) == EOF) {						// Flush
				return EOF;
			}
			
			last = __get_next_stream(last);									// Get the next stream
		}
		
		return 0;
	}
	
	flockfile(stream);														// Lock the file, call the unlocked version of this function, and in the end unlock the file
	int ret = fflush_unlocked(stream);
	funlockfile(stream);
	
	return ret;
}
