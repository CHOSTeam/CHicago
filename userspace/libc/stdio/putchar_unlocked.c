// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:25 BRT
// Last edited on January 02 of 2020, at 12:25 BRT

#include <stdio.h>

int putchar_unlocked(int c) {
	return fputc_unlocked(c, stdout);					// Call the unlocked version of fputc with stdout as the argument
}
