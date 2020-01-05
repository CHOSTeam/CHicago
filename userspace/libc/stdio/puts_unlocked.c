// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:25 BRT
// Last edited on January 02 of 2020, at 12:26 BRT

#include <stdio.h>

int puts_unlocked(const char *s) {
	return fputs_unlocked(s, stdout);					// Call the unlocked version of fputs with stdout as the argument
}
