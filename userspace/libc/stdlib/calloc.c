// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 17:05 BRT
// Last edited on January 01 of 2020, at 17:06 BRT

#include <stdlib.h>

void *calloc(size_t nmemb, size_t size) {
	return malloc(nmemb * size);							// Just redirect to malloc...
}
