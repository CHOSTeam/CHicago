// WARNING: This is a dummy stdio.h just for compiling the libgcc, DON'T USE IT!

#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0

typedef struct { int unused; } FILE;

#ifdef __cplusplus
extern "C" {
#endif

extern FILE* stderr;

#define stderr stderr

static int fclose(FILE* a) { return 0; }
static int fflush(FILE* a) { return 0; }
static FILE* fopen(const char* a, const char* b) { return 0; }
static int fprintf(FILE* a, const char* b, ...) { return 0; }
static size_t fread(void* a, size_t b, size_t c, FILE* d) { return 0; }
static int fseek(FILE* a, long b, int c) { return 0; }
static long ftell(FILE* a) { return 0; }
static size_t fwrite(const void* a, size_t b, size_t c, FILE* d) { }
static void setbuf(FILE* a, char* b) { }
static int vfprintf(FILE* a, const char* b, va_list c) { return 0; }

#ifdef __cplusplus
}
#endif

#endif