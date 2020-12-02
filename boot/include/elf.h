/* File author is Ítalo Lima Marconato Matias
 *
 * Created on October 25 of 2018, at 20:11 BRT
 * Last edited on November 29 of 2020, at 20:44 BRT */

#ifndef __ELF_H__
#define __ELF_H__

#include <efi.h>
#include <efilib.h>

#ifdef _WIN64
#define ELF_MACHINE 0x3E

typedef struct {
	UINT8 Ident[16];
	UINT16 Type;
	UINT16 Machine;
	UINT32 Version;
	UINT64 Entry;
	UINT64 ProgHeaderOffset;
	UINT64 SectHeaderOffset;
	UINT32 Flags;
	UINT16 HeaderSize;
	UINT16 ProgHeaderEntSize;
	UINT16 ProgHeaderCount;
	UINT16 SectHeaderEntSize;
	UINT16 SectHeaderCount;
	UINT16 StrSectHeaderIndex;
} ELF_HEADER;

typedef struct {
	UINT32 Name;
	UINT32 Type;
	UINT64 Flags;
	UINT64 Address;
	UINT64 Offset;
	UINT64 Size;
	UINT32 Link;
	UINT32 Info;
	UINT64 Align;
	UINT64 EntSize;
} ELF_SHDR;

typedef struct {
	UINT32 Type;
	UINT32 Flags;
	UINT64 Offset;
	UINT64 VirtAddress;
	UINT64 PhysAddress;
	UINT64 FileSize;
	UINT64 MemSize;
	UINT64 Align;
} ELF_PHDR;

typedef struct {
	UINT32 Name;
	UINT8 Info;
	UINT8 Other;
	UINT16 SectIndex;
	UINT64 Value;
	UINT64 Size;
} ELF_SYMBOL;
#else
#define ELF_MACHINE 0x03

typedef struct {
	UINT8 Ident[16];
	UINT16 Type;
	UINT16 Machine;
	UINT32 Version;
	UINT32 Entry;
	UINT32 ProgHeaderOffset;
	UINT32 SectHeaderOffset;
	UINT32 Flags;
	UINT16 HeaderSize;
	UINT16 ProgHeaderEntSize;
	UINT16 ProgHeaderCount;
	UINT16 SectHeaderEntSize;
	UINT16 SectHeaderCount;
	UINT16 StrSectHeaderIndex;
} ELF_HEADER;

typedef struct {
	UINT32 Name;
	UINT32 Type;
	UINT32 Flags;
	UINT32 Address;
	UINT32 Offset;
	UINT32 Size;
	UINT32 Link;
	UINT32 Info;
	UINT32 Align;
	UINT32 EntSize;
} ELF_SHDR;

typedef struct {
	UINT32 Type;
	UINT32 Offset;
	UINT32 VirtAddress;
	UINT32 PhysAddress;
	UINT32 FileSize;
	UINT32 MemSize;
	UINT32 Flags;
	UINT32 Align;
} ELF_PHDR;

typedef struct {
	UINT32 Name;
	UINT32 Value;
	UINT32 Size;
	UINT8 Info;
	UINT8 Other;
	UINT16 SectIndex;
} ELF_SYMBOL;
#endif

#endif
