// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 15:26 BRT
// Last edited on January 03 of 2020, at 23:39 BRT

#include <stdio.h>
#include <stdlib.h>

void __assert(const char *msg, const char *filename, int line) {
	fprintf(stderr, "Assertion `%s` at %s:%d failed\n", msg, filename, line);
	abort();
}
