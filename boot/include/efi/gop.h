/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 04 of 2021, at 17:28 BRT
 * Last edited on January 23 of 2021 at 20:36 BRT */

#pragma once

#include <efi/types.h>

#define EFI_GRAPHICS_OUTPUT_GUID { 0x9042A9DE, 0x23DC, 0x4A38, \
                                   { 0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A } }
#define EFI_EDID_DISCOVERED_GUID { 0x1C0C34F6, 0xD380, 0x41FA, \
                                   { 0xA0, 0x49, 0x8A, 0xD0, 0x6C, 0x1A, 0x66, 0xAA } }
#define EFI_EDID_ACTIVE_GUID { 0xBD8C1056, 0x9F36, 0x44EC, \
                               { 0x92, 0xA8, 0xA6, 0x33, 0x7F, 0x81, 0x79, 0x86 } }
#define EFI_EDID_OVERRIDE_GUID { 0x48ECB431, 0xFB72, 0x45C0, \
                                 { 0xA9, 0x22, 0xF4, 0x58, 0xFE, 0x04, 0x0B, 0xD5 } }

#define EFI_EDID_OVERRIDE_DONT_OVERRIDE 0x01
#define EFI_EDID_OVERRIDE_HOT_PLUG 0x02

typedef struct _EfiGraphicsOutput EfiGraphicsOutput;
typedef struct _EfiEdidOverride EfiEdidOverride;

typedef enum {
    EfiPixelFormatRGBR8,
    EfiPixelFormatBGRR8,
    EfiPixelFormatBitMask,
    EfiPixelFormatBltOnly,
    EfiPixelFormatMax
} EfiGraphicsPixelFormat;

typedef struct {
    UInt32 RedMask, GreenMask, BlueMask,
           ReservedMask;
} EfiPixelBitMask;

typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiBltOperationMax
} EfiGraphicsOutputBltOperation;

typedef struct {
    UInt32 Version;
    UInt32 Width, Height;
    EfiGraphicsPixelFormat PixelFormat;
    EfiPixelBitMask PixelInfo;
    UInt32 PixelsPerScanLine;
} EfiGraphicsOutputModeInformation;

typedef struct {
    UInt8 Blue, Green, Red, Reserved;
} EfiGraphicsOutputBltPixel;

typedef struct {
    UInt32 MaxMode, Mode;
    EfiGraphicsOutputModeInformation *Info;
    UIntN SizeOfInfo;
    EfiPhysicalAddress FrameBufferBase;
    UIntN FrameBufferSize;
} EfiGraphicsOutputMode;

typedef EfiStatus (*_EfiGraphicsOutputQueryMode)(EfiGraphicsOutput*, UInt32, UIntN*, EfiGraphicsOutputModeInformation**);
typedef EfiStatus (*_EfiGraphicsOutputSetMode)(EfiGraphicsOutput*, UInt32);
typedef EfiStatus (*_EfiGraphicsOutputBlt)(EfiGraphicsOutput*, EfiGraphicsOutputBltPixel*, EfiGraphicsOutputBltOperation,
                                           UIntN, UIntN, UIntN, UIntN, UIntN, UIntN, UIntN);

typedef EfiStatus (*_EfiEdidOverrideGetEdid)(EfiEdidOverride*, EfiHandle*, UInt32*, UIntN*, UInt8**);

struct _EfiGraphicsOutput {
    _EfiGraphicsOutputQueryMode QueryMode;
    _EfiGraphicsOutputSetMode SetMode;
    _EfiGraphicsOutputBlt Blt;
    EfiGraphicsOutputMode *Mode;
};

struct _EfiEdidOverride {
  _EfiEdidOverrideGetEdid _GetEdid;  
};

typedef struct {
    UInt32 Size;
    UInt8 *Data;
} EfiEdid;
