// WARNING: This is a dummy stdlib.h just for compiling the libgcc, DON'T USE IT!

#ifndef _STDLIB_H
#define _STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

static void abort(void) { }
static int atexit(void (*a)(void)) { return 0; }
static int atoi(const char* a) { return 0; }
static void free(void* a) { }
static char* getenv(const char* a) { return 0; }
static void* malloc(size_t a) { return 0; }

#ifdef __cplusplus
}
#endif

#endif