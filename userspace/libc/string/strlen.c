// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:22 BRT
// Last edited on January 05 of 2020, at 16:23 BRT

#include <stddef.h>

size_t strlen(const char *s) {
	size_t ret = 0;							// Here we go... let's use a simple loop, probably GCC will optimize this, but we also should try to optimize it later
	while (*s++) ret++;
	return ret;
}
