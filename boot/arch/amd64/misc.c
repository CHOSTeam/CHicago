/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:17 BRT
 * Last edited on January 31 of 2021 at 17:50 BRT */

#include <chicago.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    /* The only feature that we can return is PML5/57-bit addressing. */

    UInt64 val;
    Asm Volatile("cpuid" : "=c"(val) : "a"(0x07), "c"(0x00) : "ebx", "edx");

    switch (Type) {
    case MenuEntryCHicago: {
        return SIA_AMD64 | ((val & 0x10000) ? SIA_AMD64_57_BITS : SIA_AMD64_48_BITS);
    }
    default: {
        return 0;
    }
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
    /* Finding the best fit here is finding the one with the correct VA size (or falling back to 48-bits VA). */

    if (Header == Null || Features == Null) {
        return Null;
    }

    UInt16 feat = ArchGetFeatures(MenuEntryCHicago);
    SiaFile *fallback = Null;

    for (UIntN i = 0; i < sizeof(Header->KernelImages) / sizeof(UInt64); i++) {
        /* If the image offset is 0, this entry doesn't exist, else, we can try checking if the entry is inside the
         * file boundary (before actually starting to check other things). */

        if (Header->KernelImages[i] < sizeof(SiaHeader) || Header->KernelImages[i] + sizeof(SiaFile) >= Size) {
            continue;
        }

        /* The header starts at the position 0 of the file, so we can do this: */

        SiaFile *file = (SiaFile*)((UIntN)Header + Header->KernelImages[i]);

        /* And now check if this file's size and offset are valid (maybe we should do the type checking first?).
         * After that, we can check if this is not some invalid dual VA entry (as our kernel doesn't support dual VA
         * on the same image for now). */

        if (!file->Offset || !file->Size || file->Offset + file->Size > Size || !file->Flags ||
            !(file->Flags & SIA_AMD64) || !((file->Flags & SIA_AMD64_48_BITS) || (file->Flags & SIA_AMD64_57_BITS)) ||
            ((file->Flags & SIA_AMD64_48_BITS) && (file->Flags & SIA_AMD64_57_BITS))) {
            continue;
        } else if (file->Flags == feat) {
            /* Perfect match! Let's return it. */

            *Features = feat;

            return file;
        } else if (file->Flags & SIA_AMD64_48_BITS) {
            /* For now we can do this, as we only have two different VA flags, and we know that if the CPU supports
             * PML5, and the image is not 57-bits, if is 48-bits, and it could also work/be the fallback. */
            
            fallback = file;
        }
    }

    if (fallback != Null) {
        *Features = SIA_AMD64 | SIA_AMD64_48_BITS;
    }

    return fallback;
}
