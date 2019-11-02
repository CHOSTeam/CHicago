// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 18:56 BRT
// Last edited on October 29 of 2019, at 19:08 BRT

#ifndef __CHICAGO_ALLOC_H__
#define __CHICAGO_ALLOC_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

UIntPtr MmAllocMemory(UIntPtr size);
Void MmFreeMemory(UIntPtr block);
UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size);
UIntPtr MmGetUsage(Void);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_ALLOC_H__
