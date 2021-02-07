/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 17:17 BRT
 * Last edited on February 07 of 2021 at 13:02 BRT */

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
    return CHGetKernel(Header, Size, SIA_ARM64, Features);
}
