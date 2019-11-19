// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 20:11 BRT
// Last edited on November 15 of 2019, at 21:31 BRT

#ifndef __CHICAGO_ELF_H__
#define __CHICAGO_ELF_H__

#include <efi.h>
#include <efilib.h>

#ifdef ARCH_64
#define ELF_MACHINE 0x3E
#define ELF_BASE 0xFFFF800000000000

typedef struct {
	UINT8 ident[16];
	UINT16 type;
	UINT16 machine;
	UINT32 version;
	UINT64 entry;
	UINT64 ph_off;
	UINT64 sh_off;
	UINT32 flags;
	UINT16 eh_size;
	UINT16 ph_ent_size;
	UINT16 ph_num;
	UINT16 sh_ent_size;
	UINT16 sh_num;
	UINT16 sh_str_ndx;
} ELF_HEADER;

typedef struct {
	UINT32 name;
	UINT32 type;
	UINT64 flags;
	UINT64 addr;
	UINT64 offset;
	UINT64 size;
	UINT32 link;
	UINT32 info;
	UINT64 align;
	UINT64 ent_size;
} ELF_SHDR;

typedef struct {
	UINT32 type;
	UINT32 flags;
	UINT64 offset;
	UINT64 vaddr;
	UINT64 paddr;
	UINT64 fsize;
	UINT64 msize;
	UINT64 align;
} ELF_PHDR;

typedef struct {
	UINT32 name;
	UINT8 info;
	UINT8 other;
	UINT16 shndx;
	UINT64 value;
	UINT64 size;
} ELF_SYMBOL;
#else
#define ELF_MACHINE 0x03
#define ELF_BASE 0xC0000000

typedef struct {
	UINT8 ident[16];
	UINT16 type;
	UINT16 machine;
	UINT32 version;
	UINT32 entry;
	UINT32 ph_off;
	UINT32 sh_off;
	UINT32 flags;
	UINT16 eh_size;
	UINT16 ph_ent_size;
	UINT16 ph_num;
	UINT16 sh_ent_size;
	UINT16 sh_num;
	UINT16 sh_str_ndx;
} ELF_HEADER;

typedef struct {
	UINT32 name;
	UINT32 type;
	UINT32 flags;
	UINT32 addr;
	UINT32 offset;
	UINT32 size;
	UINT32 link;
	UINT32 info;
	UINT32 align;
	UINT32 ent_size;
} ELF_SHDR;

typedef struct {
	UINT32 type;
	UINT32 offset;
	UINT32 vaddr;
	UINT32 paddr;
	UINT32 fsize;
	UINT32 msize;
	UINT32 flags;
	UINT32 align;
} ELF_PHDR;

typedef struct {
	UINT32 name;
	UINT32 value;
	UINT32 size;
	UINT8 info;
	UINT8 other;
	UINT16 shndx;
} ELF_SYMBOL;
#endif

#endif		// __CHICAGO_ELF_H__
