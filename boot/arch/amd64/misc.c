/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:17 BRT
 * Last edited on February 05 of 2021 at 15:16 BRT */

#include <chicago.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    switch (Type) {
    case MenuEntryCHicago: {
        return SIA_AMD64;
    }
    default: {
        return 0;
    }
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
    /* No support for PML5 for now (but the build infrastructure is already ready for when we add it). So the code here
     * is just like on x86 and arm64. */

    if (Header == Null || Features == Null) {
        return Null;
    }

    for (UIntN i = 0; i < sizeof(Header->KernelImages) / sizeof(UInt64); i++) {
        if (Header->KernelImages[i] < sizeof(SiaHeader) || Header->KernelImages[i] + sizeof(SiaFile) >= Size) {
            continue;
        }

        SiaFile *file = (SiaFile*)((UIntN)Header + Header->KernelImages[i]);

        if (!file->Offset || !file->Size || file->Offset + file->Size > Size || !(file->Flags & SIA_AMD64)) {
            continue;
        }

        *Features = SIA_AMD64;

        return file;
    }

    return Null;
}
