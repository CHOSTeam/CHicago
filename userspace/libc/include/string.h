// File author is Ítalo Lima Marconato Matias
//
// Created on January 02 of 2020, at 10:09 BRT
// Last edited on January 02 of 2020, at 11:09 BRT

#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#if defined(__cplusplus)
extern "C"
{
#endif

int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *restrict s1, const void *restrict s2, size_t n);
void *memmove(void *s1, const void *s2, size_t n);
void *memset(void *s, int c, size_t n);

char *strcat(char *restrict s1, const char *restrict s2);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *restrict s1, const char *restrict s2);
size_t strlen(const char *s);

char *strncat(char *restrict s1, const char *restrict s2, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *restrict s1, const char *restrict s2, size_t n);

#if defined(__cplusplus)
}
#endif

#endif		// _STRING_H
