/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 30 of 2021, at 11:13 BRT
 * Last edited on February 04 of 2021 at 16:34 BRT */

#pragma once

#include <efi/types.h>

typedef struct __attribute__((packed)) {
    UInt8 Ident[16];
	UInt16 Type, Machine;
	UInt32 Version;
	UIntN Entry,
	      ProgHeaderOffset, SectHeaderOffset;
	UInt32 Flags;
	UInt16 HeaderSize,
	       ProgHeaderEntSize, ProgHeaderCount,
	       SectHeaderEntSize, SectHeaderCount, StrSectHeaderIndex;
} ElfHeader;

typedef struct __attribute__((packed)) {
    UInt32 Type;
#ifdef _WIN64
    UInt32 Flags;
#endif
    UIntN Offset, VirtAddress, PhysAddress,
          FileSize, MemSize;
#ifndef _WIN64
    UInt32 Flags;
#endif
    UIntN Align;
} ElfProgHeader;
