// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 17:05 BRT
// Last edited on January 05 of 2020, at 17:30 BRT

#include <chlibc/platform.h>

void free(void *ptr) {
	__free(ptr);						// Redirect...
}
