// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 18:56 BRT
// Last edited on January 05 of 2020, at 21:59 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

#ifdef __ALLOC_INT__
#if __INTPTR_WIDTH__ == 64
#define ALLOC_BLOCK_MAGIC 0xA110CB10C43A61C0
#else
#define ALLOC_BLOCK_MAGIC 0xA110CB10
#endif

typedef struct AllocBlockStruct {
	UIntPtr magic;
	UIntPtr size;
	UIntPtr start;
	Boolean free;
	UInt8 align[16 - ((sizeof(UIntPtr) * 5 + 1) % 16)];				// We need this to make sure that the allocated pointer is aligned to a 16-bytes boundary
	struct AllocBlockStruct *next;
	struct AllocBlockStruct *prev;
} AllocBlock, *PAllocBlock;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

UIntPtr MmAllocMemory(UIntPtr size);
UIntPtr MmAllocAlignedMemory(UIntPtr size, UIntPtr align);
Void MmFreeMemory(UIntPtr block);
UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size);
UIntPtr MmGetAllocSize(UIntPtr block);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_ALLOC_H__
