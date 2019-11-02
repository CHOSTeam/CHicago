// WARNING: This is a dummy unistd.h just for compiling the libgcc, DON'T USE IT!

#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

static int execv(const char* a, char* const b[]) { return -1; }
static int execve(const char* a, char* const b[], char* const c[]) { return -1; }
static int execvp(const char* a, char* const b[]) { return -1; }
static pid_t fork(void) { return -1; }

#ifdef __cplusplus
}
#endif

#endif