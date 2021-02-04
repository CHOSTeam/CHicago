/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 30 of 2021, at 11:13 BRT
 * Last edited on January 30 of 2021 at 11:26 BRT */

#pragma once

#include <efi/types.h>

typedef struct {
    UInt8 Ident[16];
	UInt16 Type, Machine;
	UInt32 Version;
	UIntN Entry,
	      ProgHeaderOffset, SectHeaderOffset;
	UInt32 Flags;
	UInt16 HeaderSize,
	       ProgHeaderEntSize, ProgHeaderCount,
	       SectHeaderEntSize, SectHeaderCount, StrSectHeaderIndex;
} __attribute__((packed)) ElfHeader;

typedef struct {
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
} __attribute__((packed)) ElfProgHeader;
