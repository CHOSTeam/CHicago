// File author is Ítalo Lima Marconato Matias
//
// Created on December 31 of 2019, at 17:11 BRT
// Last edited on January 03 of 2020, at 10:56 BRT

#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#if defined(__cplusplus)
extern "C"
{
#endif

void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

_Noreturn void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
_Noreturn void exit(int status);
_Noreturn void _Exit(int status);
_Noreturn void quick_exit(int status);

#if defined(__cplusplus)
}
#endif

#endif		// _STDLIB_H
