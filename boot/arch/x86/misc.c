/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:26 BRT
 * Last edited on February 07 of 2021 at 13:01 BRT */

#include <chicago.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    switch (Type) {
    case MenuEntryCHicago: {
#ifdef __i386__
        return SIA_X86;
#else
        return SIA_AMD64;
#endif
    }
    default: {
        return 0;
    }
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
#ifdef __i386__
    return CHGetKernel(Header, Size, SIA_X86, Features);
#else
    return CHGetKernel(Header, Size, SIA_AMD64, Features);
#endif
}
