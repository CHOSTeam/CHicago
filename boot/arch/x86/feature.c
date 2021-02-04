/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:26 BRT
 * Last edited on January 30 of 2021 at 12:08 BRT */

#include <chicago.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    switch (Type) {
    case MenuEntryCHicago: {
        return SIA_X86;
    }
    default: {
        return 0;
    }
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
    /* x86 has no special features (at least for now), so we just need to check for SIA_X86 (no need for even saving a
     * fallback). */

    if (Header == Null || Features == Null) {
        return Null;
    }

    for (UIntN i = 0; i < sizeof(Header->KernelImages) / sizeof(UInt64); i++) {
        if (Header->KernelImages[i] < sizeof(SiaHeader) || Header->KernelImages[i] + sizeof(SiaFile) >= Size) {
            continue;
        }

        SiaFile *file = (SiaFile*)((UIntN)Header + Header->KernelImages[i]);

        if (!file->Offset || !file->Size || file->Offset + file->Size > Size || !(file->Flags & SIA_X86)) {
            continue;
        }

        /* If it is SIA_X86, it is exactly what we are searching (at least for now). */

        *Features = SIA_X86;

        return file;
    }

    return Null;
}
