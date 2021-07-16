/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 16:47 BRT
 * Last edited on July 15 of 2021 at 12:23 BRT */

#pragma once

#include <util.h>

#define SIA_MAGIC 0xC4051AF0
#define CH_BOOT_INFO_MAGIC 0xC4057D41

/* Arch-specific file flags for the kernel. */

#define SIA_X86 0x01
#define SIA_AMD64 0x02

#define CH_MEM_KCODE 0x00
#define CH_MEM_KDATA 0x01
#define CH_MEM_KDATA_RO 0x02
#define CH_MEM_DEV 0x03
#define CH_MEM_RES 0x04
#define CH_MEM_FREE 0x05

typedef struct __attribute__((packed)) {
    UInt32 Magic;
    UInt16 Info;
    UInt8 UniqueId[16];
    UInt64 FreeFileCount, FreeFileOffset, FreeDataCount, FreeDataOffset, KernelImages[16], RootImages[16];
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
    UIntN Start, End;
    Char8 *Name;
} CHBootInfoSymbol;

typedef struct __attribute__((packed)) {
    UInt64 Base, Count;
    UInt8 Type;
} CHBootInfoMemMap;

typedef struct __attribute__((packed)) {
    UInt32 Magic;
    UIntN KernelStart, PhysMgrStart, KernelEnd;
    UInt64 EfiTempAddress, MinPhysicalAddress, MaxPhysicalAddress, PhysicalMemorySize;
    Void *Directory;

    struct __attribute__((packed)) {
        Boolean Extended;
        UInt32 Size;
        UInt64 Sdt;
    } Acpi;

    struct __attribute__((packed)) {
        UIntN Count;
        CHBootInfoSymbol *Start;
    } Symbols;

    struct __attribute__((packed)) {
        UIntN Count;
        CHBootInfoMemMap Entries[256];
    } MemoryMap;

    struct __attribute__((packed)) {
        UIntN Size, Index;
        UInt8 *Data;
    } BootImage;

    struct __attribute__((packed)) {
        UIntN Width, Height, BackBuffer, FrontBuffer;
    } FrameBuffer;

    UInt8 KernelStack[0x2000];
} CHBootInfo;

SiaFile *CHGetKernel(SiaHeader*, UIntN, UInt16, UInt16*);
