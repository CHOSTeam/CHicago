/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 02 of 2021, at 17:31 BRT
 * Last edited on February 07 of 2021 at 18:40 BRT */

#pragma once

#include <efi/gop.h>
#include <efi/st.h>

typedef struct {
    Int32 Offset, Width, Height, Left, Top, Advance;
} EfiFontGlyph;

typedef struct {
    Int32 Ascender, Descender, Height;
    const EfiFontGlyph *GlyphInfo;
    const UInt8 *GlyphData;
} EfiFontData;

extern EfiHandle EfiIH;
extern const EfiSystemTable *EfiST;
extern const EfiRuntimeServices *EfiRS;
extern const EfiBootServices *EfiBS;
extern const EfiConfigurationTable *EfiCT;

extern EfiSimpleTextInput *EfiConIn;
extern EfiSimpleTextOutput *EfiConOut, EfiConErr;

extern EfiGraphicsOutput *EfiGop;

extern EfiGuid EfiDevicePathGuid;
extern EfiGuid EfiSimpleTextInputExGuid;
extern EfiGuid EfiSimpleTextInputGuid;
extern EfiGuid EfiSimpleTextOutputGuid;
extern EfiGuid EfiSimplePointerGuid;
extern EfiGuid EfiAbsolutePointerGuid;
extern EfiGuid EfiSimpleFileSystemGuid;
extern EfiGuid EfiFileInfoGuid;
extern EfiGuid EfiFileSystemInfoGuid;
extern EfiGuid EfiFileSystemLabelGuid;
extern EfiGuid EfiGraphicsOutputGuid;
extern EfiGuid EfiLoadedImageGuid;
extern EfiGuid EfiRngGuid;

extern const EfiFontData EfiFont;

EfiStatus EfiSetupGraphics(UInt8, UInt8, UInt8);
EfiStatus EfiInitLib(EfiHandle, const EfiSystemTable*, Boolean);

Void EfiFillScreen(UInt8, UInt8, UInt8);
Void EfiPutPixel(UInt16, UInt16, UInt8, UInt8, UInt8);
Void EfiDrawRectangle(UInt16, UInt16, UInt16, UInt16, UInt8, UInt8, UInt8);
Void EfiFillRectangle(UInt16, UInt16, UInt16, UInt16, UInt8, UInt8, UInt8);
Void EfiDrawCharacter(Char8, UInt16, UInt16, UInt8, UInt8, UInt8);
Void EfiGetStringMetrics(const Char8*, UInt16*, UInt16*);
Void EfiDrawString(const Char8*, UInt16, UInt16, UInt8, UInt8, UInt8);

Char8 *EfiDuplicateString8(const Char8*);
Char16 *EfiDuplicateString16(const Char16*);
Boolean EfiCompareString8(const Char8*, const Char8*);
Boolean EfiCompareString16(const Char16*, const Char16*);
IntN EfiToLower(IntN);

Void *EfiSetMemory8(Void*, UIntN, UInt8);
Void *EfiSetMemory16(Void*, UIntN, UInt16);
Void *EfiSetMemory32(Void*, UIntN, UInt32);
Void *EfiSetMemory64(Void*, UIntN, UInt64);
Void *EfiZeroMemory(Void*, UIntN);
Void *EfiCopyMemory(Void*, const Void*, UIntN);

Void *EfiAllocatePool(UIntN);
Void *EfiAllocateZeroPool(UIntN);
Void *EfiAllocateCopyPool(const Void*, UIntN);
Void *EfiReallocatePool(Void*, UIntN, UIntN);
EfiPhysicalAddress EfiAllocatePages(UIntN);
Void EfiFreePool(Void*);
Void EfiFreePages(EfiPhysicalAddress, UIntN);

EfiMemoryDescriptor *EfiGetMemoryMap(UIntN*, UIntN*, UIntN*, UInt32*);

EfiStatus EfiOpenFile(Char16*, UInt8, EfiFile**);
UIntN EfiGetFileSize(EfiFile*);
