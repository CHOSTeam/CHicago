// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 12:24 BRT
// Last edited on January 02 of 2020, at 12:25 BRT

#include <stdio.h>

int getchar_unlocked(void) {
	return fgetc_unlocked(stdin);						// Call the unlocked version of fgetc with stdin as the argument
}
