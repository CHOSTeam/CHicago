// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:19 BRT
// Last edited on January 02 of 2020, at 12:20 BRT

#include <stdio.h>

int puts(const char *s) {
	return fputs(s, stdout);					// Call fputs with stdout as the argument
}
