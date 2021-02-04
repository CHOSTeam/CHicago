/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 23 of 2021, at 17:07 BRT
 * Last edited on January 23 of 2021 at 20:26 BRT */

#pragma once

#include <efi/dp.h>
#include <efi/st.h>

#define EFI_LOADED_IMAGE_GUID { 0x5B1B31A1, 0x9562, 0x11D2, \
                                { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }

#define EFI_LOADED_IMAGE_REV 0x1000

typedef struct _EfiLoadedImage EfiLoadedImage;

struct _EfiLoadedImage {
    UInt32 Revision;
    EfiHandle ParentHandle;
    EfiSystemTable *ST;
    EfiHandle DeviceHandle;
    EfiDevicePath *FilePath;
    Void *Reserved;
    UInt32 LoadOptionsSize;
    Void *LoadOptions;
    Void *ImageBase;
    UInt64 ImageSize;
    EfiMemoryType ImageCodeType;
    EfiMemoryType ImageDataType;
    _EfiImageUnload Unload;
};
