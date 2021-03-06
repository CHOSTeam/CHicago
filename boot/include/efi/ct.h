/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 03 of 2021, at 13:29 BRT
 * Last edited on March 11 of 2021 at 18:54 BRT */

#pragma once

#include <efi/types.h>

/* ConfigurationTable->VendorGuid values. */

#define EFI_ACPI_10_TABLE_GUID { 0xEB9D2D30, 0x2D88, 0x11D3, \
                                 { 0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }
#define EFI_ACPI_20_TABLE_GUID { 0x8868E871, 0xE4F1, 0x11D3, \
                                 { 0xBC, 0x22, 0x00, 0x80, 0xC7, 0x3C, 0x88, 0x81 } }
#define EFI_SAL_SYSTEM_TABLE_GUID { 0xEB9D2D32, 0x2D88, 0x11D3, \
                                    { 0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }
#define EFI_SMBIOS_TABLE_GUID { 0xEB9D2D31, 0x2D88, 0x11D3, \
                                { 0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }
#define EFI_SMBIOS3_TABLE_GUID { 0xF2FD1544, 0x9794, 0x4A2C, \
                                 { 0x99, 0x2E, 0xE5, 0xBB, 0xCF, 0x20, 0xE3, 0x94 } }
#define EFI_MPS_TABLE_GUID { 0xEB9D2D2F, 0x2D88, 0x11D3, \
                             { 0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }

typedef struct {
    Char8 Signature[8];
    UInt8 Checksum, OemID[6], Revision;
    UInt32 RsdtAddress, Length;
    UInt64 XsdtAddress;
    UInt8 ExtraChecksum, Reserved[3];
} AcpiRsdp;

typedef struct {
    EfiGuid VendorGuid;
    Void *VendorTable;
} EfiConfigurationTable;
