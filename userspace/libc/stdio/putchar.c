// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:19 BRT
// Last edited on January 02 of 2020, at 12:20 BRT

#include <stdio.h>

int putchar(int c) {
	return fputc(c, stdout);					// Call fputc with stdout as the argument
}
