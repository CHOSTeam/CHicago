/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:17 BRT
 * Last edited on February 04 of 2021 at 12:30 BRT */

#include <chicago.h>
#include <efi/lib.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    /* The only feature that we can return is if we support 48-bits addressing. */

    UInt64 val;    
    Asm Volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(val));

    switch (Type) {
    case MenuEntryCHicago: {
        /* CHicago needs (at least for now) 4KB granularity support. */

        return (val >> 28) & 0x0F ? 0 :
               (SIA_ARM64 | ((val & 0x0F) >= 5 ? SIA_ARM64_39_BITS : SIA_ARM64_48_BITS));
    }
    default: {
        return 0;
    }
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
    /* Same find the best VA fit or fallback as in the amd64 code (but here the fallback is 39-bits). */

    if (Header == Null || Features == Null) {
        return Null;
    }

    UInt16 feat = ArchGetFeatures(MenuEntryCHicago);
    SiaFile *fallback = Null;

    for (UIntN i = 0; i < sizeof(Header->KernelImages) / sizeof(UInt64); i++) {
        if (Header->KernelImages[i] < sizeof(SiaHeader) || Header->KernelImages[i] + sizeof(SiaFile) >= Size) {
            continue;
        }

        SiaFile *file = (SiaFile*)((UIntN)Header + Header->KernelImages[i]);

        /* This is the place where the code diverges, as we need to check for SIA_ARM64 and the SIA_ARM64_39/48_BITS
         * flags. */

        if (!file->Offset || !file->Size || file->Offset + file->Size > Size || !file->Flags ||
            !(file->Flags & SIA_ARM64) || !((file->Flags & SIA_ARM64_39_BITS) || (file->Flags & SIA_ARM64_48_BITS)) ||
            ((file->Flags & SIA_ARM64_39_BITS) && (file->Flags & SIA_ARM64_48_BITS))) {
            continue;
        } else if (file->Flags == feat) {
            *Features = feat;
            return file;
        } else if (file->Flags & SIA_ARM64_39_BITS) {
            fallback = file;
        }
    }

    if (fallback != Null) {
        *Features = SIA_ARM64 | SIA_ARM64_39_BITS;
    }

    return fallback;
}
