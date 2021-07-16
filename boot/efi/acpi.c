/* File author is Ítalo Lima Marconato Matias
 *
 * Created on March 10 of 2021, at 17:26 BRT
 * Last edited on July 06 of 2021 at 20:14 BRT */

#include <efi/lib.h>

EfiPhysicalAddress EfiGetAcpiTables(Boolean *Extended) {
    /* The System Table ->CT (and ->NumberOfTableEntries) is everything we need (to search for the RSDP). We just need
     * to remember that if we can find an ACPI 1.0 RSDP, we should keep on searching (as we may find an ACPI 2.0
     * RSDP). */

    EfiPhysicalAddress rsdt = 0;

    for (UIntN i = 0; i < EfiST->NumberOfTableEntries; i++) {
        if (EfiCompareMemory(&EfiCT[i].VendorGuid, &EfiAcpi20TableGuid, sizeof(EfiGuid))) {
            rsdt = ((AcpiRsdp*)EfiCT[i].VendorTable)->XsdtAddress;
            *Extended = True;
            break;
        } else if (!rsdt && EfiCompareMemory(&EfiCT[i].VendorGuid, &EfiAcpi10TableGuid, sizeof(EfiGuid)))
            rsdt = ((AcpiRsdp*)EfiCT[i].VendorTable)->RsdtAddress, *Extended = False;
    }

    return rsdt;
}
