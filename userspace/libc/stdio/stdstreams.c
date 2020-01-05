// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 21:56 BRT
// Last edited on January 05 of 2020, at 13:51 BRT

#include <chicago/process.h>

#define __STDIO__
#include <stdio.h>
#include <stdlib.h>

extern int __fopen_parse_mode(const char *mode);

static char __stdout_buf[BUFSIZ];

FILE *stdin = NULL;
FILE *stdout = &((FILE){ 0 });
FILE *stderr = &((FILE){ 0 });

static void __init_stream(FILE *restrict stream, char *mode, IntPtr file, IntPtr lock, char *buf) {
	stream->file = file;														// Set the file handle
	stream->lock = lock == -1 ? PsCreateLock() : lock;							// Set/create the lock
	
	if (stream->lock == -1) {
		abort();																// Failed, abort the program...
	}
	
	stream->buf = buf;															// Set the buffer
	stream->buf_size = buf == NULL ? 0 : BUFSIZ;								// The buffer size
	stream->unget = EOF;														// For now we don't have anything to unget
	stream->flags = (buf == NULL ? _IONBF : _IOLBF) | __fopen_parse_mode(mode);	// Set the flags (if we should buffer the input/output)
}

void __libc_init_streams(void) {
	__init_stream(stdout, "w", -1, -1, __stdout_buf);							// Init stdout
	__init_stream(stderr, "w", -1, stdout->lock, NULL);							// Init stderr
}
