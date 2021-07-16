/* File author is Ítalo Lima Marconato Matias
 *
 * Created on July 04 of 2021, at 12:23 BRT
 * Last edited on July 10 of 2021 at 20:10 BRT */

#pragma once

#include <efi/types.h>

#define MAP_RES 0x00
#define MAP_FREE 0x01
#define MAP_KERNEL 0x02

#define MAP_VIRT 0x01
#define MAP_EXEC 0x02
#define MAP_WRITE 0x04
#define MAP_DEVICE 0x08

typedef struct _Mapping {
    UInt64 Size;
    UInt8 Type, Flags;
    EfiVirtualAddress Virtual;
    EfiPhysicalAddress Physical;
    struct _Mapping *Next, *Prev;
} Mapping;

Mapping *AddMapping(Mapping*, EfiVirtualAddress, EfiPhysicalAddress*, UInt64, UInt8);
Mapping *InitMappings(Void);
