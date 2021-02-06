/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:17 BRT
 * Last edited on February 05 of 2021 at 20:30 BRT */

#include <chicago.h>
#include <efi/lib.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    /* Everything we have to do is check if we support 4KiB pages (as we need them, at least for now). */

    UInt64 val;    
    asm volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(val));

    switch (Type) {
    case MenuEntryCHicago: {
        return (val >> 28) & 0x0F ? 0 : SIA_ARM64;
    }
    default: {
        return 0;
    }
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
    /* Like on x86, here we only support 48-bits addressing (the documentation at least says that we can assume every
     * arm64 board will support it). */

    if (Header == Null || Features == Null) {
        return Null;
    }

    for (UIntN i = 0; i < sizeof(Header->KernelImages) / sizeof(UInt64); i++) {
        if (Header->KernelImages[i] < sizeof(SiaHeader) || Header->KernelImages[i] + sizeof(SiaFile) >= Size) {
            continue;
        }

        SiaFile *file = (SiaFile*)((UIntN)Header + Header->KernelImages[i]);

        if (!file->Offset || !file->Size || file->Offset + file->Size > Size || !(file->Flags & SIA_ARM64)) {
            continue;
        }

        *Features = SIA_ARM64;

        return file;
    }

    return Null;
}
