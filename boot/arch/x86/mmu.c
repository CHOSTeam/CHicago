/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 22:53 BRT
 * Last edited on February 07 of 2021 at 13:37 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static inline UInt32 MmuMakeEntry(EfiPhysicalAddress Physical, UInt8 Type) {
    return Physical | MMU_PRESENT | ((Type != CH_MEM_KCODE && Type != CH_MEM_KDATA_RO) ? MMU_WRITE : 0);
}

static Boolean MmuIsPresent(UInt32 Table) {
    return Table & MMU_PRESENT;
}

static Boolean MmuIsHuge(UInt32 Table) {
    return Table & MMU_HUGE;
}

static EfiStatus MmuMap(Void *Directory, CHMapping **List, CHMapping *Entry) {
    if (Entry->Type == CH_MEM_MMU) {
        return EFI_SUCCESS;
    }

    /* Simillar to the arm64 code (and the amd64 code), but we only have two levels to worry about (the PD and the
     * PT). */

    EfiStatus status;
    UIntN start = 0, size = Entry->Size, level;

    /* First level is 4MiB, let's map what we can using it. */

s:  level = (UIntN)Directory;

    while (!(((Entry->Virtual + start) & 0x3FFFFF) || ((Entry->Physical + start) & 0x3FFFFF)) && size >= 0x400000) {
        ((UInt32*)level)[((Entry->Virtual + start) >> 22) & 0x3FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type) | MMU_HUGE;
        start += 0x400000;
        size -= 0x400000;
    }

    if (!size) {
        return EFI_SUCCESS;
    }

    /* We still have memory left to map, let's use 4KiB pages. */

    if (EFI_ERROR((status = CHWalkMmuLevel((UInt32*)level, List, Entry->Virtual + start, 22, 0x3FF,
                                           MMU_PRESENT | MMU_WRITE, MmuIsPresent, MmuIsHuge, &level)))) {
        return status;
    }

    while (size) {
        ((UInt32*)level)[((Entry->Virtual + start) >> 12) & 0x3FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type);
        start += 0x1000;
        size -= 0x1000;

        /* Remembering to goto s if we step into another PDE. */

        if ((((Entry->Virtual + start) >> 22) & 0x3FF) != (((Entry->Virtual + start - 0x1000) >> 22) & 0x3FF)) {
            goto s;
        }
    }

    return EFI_SUCCESS;
}

EfiStatus ArchInitCHicagoMmu(UInt16, CHMapping **List, Void **Out) {
    if (List == Null || *List == Null || Out == Null) {
        return EFI_INVALID_PARAMETER;
    }

    /* Alloc the page dir and convert it into a pointer (for MmuMap). */

    EfiStatus status;
    EfiPhysicalAddress addr;

    if ((*List = CHAddMapping(*List, UINT32_MAX, 0x1000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
        return EFI_OUT_OF_RESOURCES;
    }

    UInt32 *pd = *Out = (UInt32*)addr;

    /* Clean the pagedir, map the jump function, and finally start mapping everything (we probably could and should make
     * this part of the function arch-independent, as it already is arch-indenpendent). */

    EfiZeroMemory(pd, 0x1000);

    if (EFI_ERROR((status = CHMapKernel(pd, List, MmuMap)))) {
        return status;
    }

    /* Last but not least: Create the recursive entry. */

    pd[1023] = addr | MMU_PRESENT | MMU_WRITE;

    return EFI_SUCCESS;
}
