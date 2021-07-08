/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:26 BRT
 * Last edited on July 06 of 2021 at 20:07 BRT */

#include <chicago.h>
#include <efi/lib.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType Type) {
    /* We need to check if the features that we need/want are available (PSE, FMA, SSE4, XSAVE and AVX2).
     * The info is stored across two different CPUID functions (0x01=Processor Info/Feature Bits, 0x07=Extended
     * Features). */

    UInt32 ebx, ecx, edx;
    asm volatile("cpuid" : "=c"(ecx), "=d"(edx) : "a"(0x01) : "%ebx");
    asm volatile("cpuid" : "=b"(ebx) : "a"(0x07), "c"(0x00) : "%edx");

    if (!(edx & 0x08)) {
        EfiDrawString("This CPU doesn't support PSE.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return 0;
    } else if (!(ecx & 0x1000)) {
        EfiDrawString("This CPU doesn't support FMA.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return 0;
    } else if (!(ecx & 0x100000)) {
        EfiDrawString("This CPU doesn't support SSE4.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return 0;
    } else if (!(ecx & 0x4000000)) {
        EfiDrawString("This CPU doesn't support XSAVE.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return 0;
    } else if (!(ebx & 0x20)) {
        EfiDrawString("This CPU doesn't support AVX2.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return 0;
    }

    switch (Type) {
    case MenuEntryCHicago:
#ifdef __i386__
        return SIA_X86;
#else
        return SIA_AMD64;
#endif
    default: return 0;
    }
}

SiaFile *ArchGetBestFitCHicago(SiaHeader *Header, UIntN Size, UInt16 *Features) {
#ifdef __i386__
    return CHGetKernel(Header, Size, SIA_X86, Features);
#else
    return CHGetKernel(Header, Size, SIA_AMD64, Features);
#endif
}
