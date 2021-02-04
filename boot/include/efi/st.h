/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 02 of 2021, at 12:36 BRT
 * Last edited on January 23 of 2021 at 20:30 BRT */

#pragma once

#include <efi/simple_io.h>
#include <efi/bs.h>
#include <efi/ct.h>
 
#define EFI_SYSTEM_TABLE_REV_1_02 ((1 << 16) | 02)
#define EFI_SYSTEM_TABLE_REV_1_10 ((1 << 16) | 10)
#define EFI_SYSTEM_TABLE_REV_2_00 ((2 << 16) | 00)
#define EFI_SYSTEM_TABLE_REV_2_10 ((2 << 16) | 10)
#define EFI_SYSTEM_TABLE_REV_2_20 ((2 << 16) | 20)
#define EFI_SYSTEM_TABLE_REV_2_30 ((2 << 16) | 30)
#define EFI_SYSTEM_TABLE_REV_2_31 ((2 << 16) | 31)
#define EFI_SYSTEM_TABLE_REV_2_40 ((2 << 16) | 40)
#define EFI_SYSTEM_TABLE_REV_2_50 ((2 << 16) | 50)
#define EFI_SYSTEM_TABLE_REV_2_60 ((2 << 16) | 60)
#define EFI_SYSTEM_TABLE_REV_2_70 ((2 << 16) | 70)
#define EFI_SYSTEM_TABLE_REV_2_80 ((2 << 16) | 80)
#define EFI_SYSTEM_TABLE_SIGNATURE 0x5453595320494249
#define EFI_SYSTEM_TABLE_REVISION EFI_SYSTEM_TABLE_REV_2_80
#define EFI_SPEC_VERSION EFI_SYSTEM_TABLE_REVISION

typedef struct {
    EfiTableHeader Hdr;
    Char16 *Vendor;
    UInt32 Revision;
    EfiHandle ConInHandle;
    EfiSimpleTextInput *ConIn;
    EfiHandle ConOutHandle;
    EfiSimpleTextOutput *ConOut;
    EfiHandle ConErrHandle;
    EfiSimpleTextOutput *ConErr;
    EfiRuntimeServices *RS;
    EfiBootServices *BS;
    UIntN NumberOfTableEntries;
    EfiConfigurationTable *CT;
} EfiSystemTable;
