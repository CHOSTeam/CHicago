// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:09 BRT
// Last edited on November 01 of 2019, at 16:45 BRT

#ifndef __CHICAGO_VIRT_H__
#define __CHICAGO_VIRT_H__

#include <chicago/types.h>

#ifndef MM_PAGE_SIZE
#define MM_PAGE_SIZE 0x1000
#endif

#define VIRT_FLAGS_HIGHEST 0x01
#define VIRT_FLAGS_AOR 0x02

#define VIRT_PROT_READ 0x04
#define VIRT_PROT_WRITE 0x08
#define VIRT_PROT_EXEC 0x10

#ifdef __cplusplus
extern "C"
{
#endif

UIntPtr VirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags);
Boolean VirtFreeAddress(UIntPtr addr, UIntPtr size);
UInt32 VirtQueryProtection(UIntPtr addr);
Boolean VirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags);
UIntPtr VirtGetUsage(Void);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_VIRT_H__
