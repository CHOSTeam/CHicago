/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 02 of 2021, at 15:52 BRT
 * Last edited on January 23 of 2021 at 20:23 BRT */

#pragma once

#include <efi/types.h>

#define EFI_DEVICE_PATH_GUID { 0x9576E91, 0x6D3F, 0x11D2, \
                               { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }

typedef struct {
    UInt8 Type, SubType;
    UInt8 Length[2];
} EfiDevicePath;
