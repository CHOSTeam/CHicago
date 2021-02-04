/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 16:47 BRT
 * Last edited on February 04 of 2021 at 15:15 BRT */

#pragma once

#include <efi/types.h>

#define SIA_MAGIC 0xC4051AF0
#define CH_BOOT_INFO_MAGIC 0xC4057D41

#define SIA_FIXED 0x01

/* Arch-specific file flags for the kernel. */

#define SIA_X86 0x01

#define SIA_ARM64 0x02
#define SIA_ARM64_39_BITS 0x08
#define SIA_ARM64_48_BITS 0x10

#define SIA_AMD64 0x04
#define SIA_AMD64_48_BITS 0x08
#define SIA_AMD64_57_BITS 0x10

/* And file flags for normal files/directories. */

#define SIA_DIR 0x01
#define SIA_READ 0x02
#define SIA_WRITE 0x04
#define SIA_EXEC 0x08

#define CH_MEM_KCODE 0x00
#define CH_MEM_KDATA 0x01
#define CH_MEM_MMU 0x02
#define CH_MEM_DEV 0x03
#define CH_MEM_RES 0x04
#define CH_MEM_FREE 0x05

typedef struct _CHMapping CHMapping;

typedef struct __attribute__((packed)) {
    UInt32 Magic;
    UInt16 Info;
    UInt8 UniqueId[16];
    UInt64 FreeFileCount, FreeFileOffset,
           FreeDataCount, FreeDataOffset,
           KernelImages[16], RootImages[16];
} SiaHeader;

typedef struct __attribute__((packed)) {
    UInt64 Next;
    UInt16 Flags;
    UInt64 Size, Offset;
    Char8 Name[64];
} SiaFile;

typedef struct __attribute__((packed)) {
    UInt64 Next;
    UInt8 Data[504];
} SiaData;

typedef struct __attribute__((packed)) {
    UIntN Base, Size;
    UInt8 Type;
} CHBootInfoMemMap;

typedef struct __attribute__((packed)) {
    UInt32 Magic;
    UIntN KernelStart, RegionsStart, KernelEnd,
          EfiTempAddress;
    UInt64 MinPhysicalAddress, MaxPhysicalAddress, PhysicalMemorySize;
    Void *Directory;

    struct __attribute__((packed)) {
        UIntN Count;
        CHBootInfoMemMap *Entries;
    } MemoryMap;

    struct __attribute__((packed)) {
        UIntN Size, Index;
        Char8 *Data;
    } BootImage;

    struct __attribute__((packed)) {
        UIntN Width, Height, Size, Address;
    } FrameBuffer;

    UInt8 KernelStack[0x10000];
} CHBootInfo;

struct _CHMapping {
    EfiVirtualAddress Virtual;
    EfiPhysicalAddress Physical;
    UIntN Size;
    UInt8 Type;
    CHMapping *Prev, *Next;
};

CHMapping *CHAddMapping(CHMapping*, EfiVirtualAddress, UIntN, UInt8, EfiPhysicalAddress*, Boolean);
