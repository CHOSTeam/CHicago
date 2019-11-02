// WARNING: This is a dummy string.h just for compiling the libgcc, DON'T USE IT!

#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static void* memcpy(void* a, const void* b, size_t c) { return a; }
static void* memset(void* a, int b, size_t c) { return a; }
static char* strcpy(char* a, const char* b) { return a; }
static size_t strlen(const char* a) { return 0; }

#ifdef __cplusplus
}
#endif

#endif