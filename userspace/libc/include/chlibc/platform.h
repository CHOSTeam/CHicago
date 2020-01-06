// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:26 BRT
// Last edited on January 05 of 2020, at 22:49 BRT

#ifndef _CHLIBC_PLATFORM_H
#define _CHLIBC_PLATFORM_H

#include <chlibc/__platform.h>

#include <stddef.h>

#ifdef __ALLOC_GENERIC__
#if __INTPTR_WIDTH__ == 64
#define ALLOC_BLOCK_MAGIC 0xA110CB10C43A61C0
#define UINTPTR unsigned long long
#else
#define ALLOC_BLOCK_MAGIC 0xA110CB10
#define UINTPTR unsigned int
#endif

typedef struct alloc_block {
	UINTPTR magic;
	UINTPTR size;
	UINTPTR start;
	char free;
	char align[16 - ((sizeof(UINTPTR) * 5 + 1) % 16)];				// We need this to make sure that the allocated pointer is aligned to a 16-bytes boundary
	struct alloc_block *next;
	struct alloc_block *prev;
} alloc_block_t;
#endif

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

void *__alloc_virt(size_t size);
void __free_virt(void *addr, size_t size);

void *__alloc(size_t size);
void *__aalloc(size_t align, size_t size);
void __free(void *ptr);
void *__realloc(void *ptr, size_t size);

void __exit(int status);

#endif		// _CHLIBC_PLATFORM_H
