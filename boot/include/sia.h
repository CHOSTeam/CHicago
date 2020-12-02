/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 21 of 2020, at 00:02 BRT
 * Last edited on October 24 of 2020, at 14:34 BRT */

#ifndef __SIA_H__
#define __SIA_H__

#include <loader.h>

#define SIA_MAGIC 0xC4051AF0
#define SIA_INFO_KERNEL 0x01
#define SIA_FLAGS_DIRECTORY 0x01
#define SIA_FLAGS_READ 0x02
#define SIA_FLAGS_WRITE 0x04
#define SIA_FLAGS_EXEC 0x08

typedef struct {
	UINT32 Magic;
	UINT16 Info;
	UINT8 UUID[16];
	UINT64 FreeFileCount;
	UINT64 FreeFileOffset;
	UINT64 FreeDataCount;
	UINT64 FreeDataOffset;
	UINT64 KernelOffset;
	UINT64 RootOffset;
} __attribute__((packed)) SIA_HEADER;

typedef struct {
	UINT64 Next;
	UINT16 Flags;
	UINT64 Size;
	UINT64 Offset;
	CHAR8 Name[64];
} __attribute__((packed)) SIA_FILE;

typedef struct {
	UINT64 Next;
	UINT8 Contents[504];
} __attribute__((packed)) SIA_DATA;

EFI_STATUS SiaLoadKernel(CHAR8 *Buffer, UINTN Size, UINTN *Entry, UINTN *StartPhys, UINTN *StartVirt,
						 UINTN *EndPhys, UINTN *EndVirt);

#endif
