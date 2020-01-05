// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:26 BRT
// Last edited on January 05 of 2020, at 17:41 BRT

#ifndef _CHLIBC_PLATFORM_H
#define _CHLIBC_PLATFORM_H

#include <chlibc/__platform.h>

#include <stddef.h>

__platform_fname_t __get_fname(const char *filename);
__platform_file_t __open(__platform_fname_t fname, int mode);
__platform_lock_t __create_lock(void);

void __close(__platform_file_t file);
void __remove_lock(__platform_lock_t lock);

int __add_stream(void *stream);
void __remove_stream(void *stream);
void *__get_stream_list(void);
void *__get_stream(void *cur);
void *__get_next_stream(void *cur);

void __lock(__platform_lock_t lock);
int __try_lock(__platform_lock_t lock);
void __unlock(__platform_lock_t lock);

size_t __read(__platform_file_t file, size_t size, void *buf);
size_t __write(__platform_file_t file, size_t size, const void *buf);
void __seek(__platform_file_t file, long int offset, int whence);
long int __tell(__platform_file_t file);

void *__alloc(size_t size);
void __free(void *ptr);
void *__realloc(void *ptr, size_t size);

void __exit(int status);

#endif		// _CHLIBC_PLATFORM_H
