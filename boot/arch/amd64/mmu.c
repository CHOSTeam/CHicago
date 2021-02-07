/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 28 of 2021, at 09:16 BRT
 * Last edited on February 07 of 2021 at 13:32 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static inline UInt64 MmuMakeEntry(EfiPhysicalAddress Physical, UInt8 Type) {
    return Physical | MMU_PRESENT | ((Type != CH_MEM_KCODE && Type != CH_MEM_KDATA_RO) ? MMU_WRITE : 0) |
                                    (Type == CH_MEM_KCODE ? 0 : MMU_NO_EXEC);
}

static Boolean MmuIsPresent(UInt64 Table) {
    return Table & MMU_PRESENT;
}

static Boolean MmuIsHuge(UInt64 Table) {
    return Table & MMU_HUGE;
}

static EfiStatus MmuMap(Void *Directory, CHMapping **List, CHMapping *Entry) {
    if (Entry->Type == CH_MEM_MMU) {
        return EFI_SUCCESS;
    }

    EfiStatus status;
    UIntN start = 0, size = Entry->Size, level;

    /* Skip out the first levels (512GB and 1GB), as we don't support mapping huge pages with them */

s:  level = (UInt64)Directory;

    if (EFI_ERROR((status = CHWalkMmuLevel((UInt64*)level, List, Entry->Virtual + start, 39, 0x1FF,
                                           MMU_PRESENT | MMU_WRITE, MmuIsPresent, MmuIsHuge, &level)))) {
        return status;
    } else if (EFI_ERROR((status = CHWalkMmuLevel((UInt64*)level, List, Entry->Virtual + start, 30, 0x1FF,
                                                  MMU_PRESENT | MMU_WRITE, MmuIsPresent, MmuIsHuge, &level)))) {
        return status;
    }

    /* Now the rest is exactly like arm64 (except for the page flags, though that is handled on MmuMakeEntry). And by
     * that I mean that first we map what we can with 2MiB pages (jumping to the beginning if needed), and then we map
     * using 4KiB pages. */

    while (!(((Entry->Virtual + start) & 0x1FFFFF) || ((Entry->Physical + start) & 0x1FFFFF)) && size >= 0x200000) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 21) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type) | MMU_HUGE;
        start += 0x200000;
        size -= 0x200000;

        if ((((Entry->Virtual + start) >> 30) & 0x1FF) != (((Entry->Virtual + start - 0x200000) >> 30) & 0x1FF)) {
            goto s;
        }
    }

    if (EFI_ERROR((status = CHWalkMmuLevel((UInt64*)level, List, Entry->Virtual + start, 21, 0x1FF,
                                           MMU_PRESENT | MMU_WRITE, MmuIsPresent, MmuIsHuge, &level)))) {
        return status;
    }

    while (size) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 12) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type);
        start += 0x1000;
        size -= 0x1000;

        if ((((Entry->Virtual + start) >> 21) & 0x1FF) != (((Entry->Virtual + start - 0x1000) >> 21) & 0x1FF)) {
            goto s;
        }
    }

    return EFI_SUCCESS;
}

EfiStatus ArchInitCHicagoMmu(UInt16, CHMapping **List, Void **Out) {
    if (List == Null || *List == Null || Out == Null) {
        return EFI_INVALID_PARAMETER;
    }

    /* Allocate the PML4 pointer. */

    EfiStatus status;
    EfiPhysicalAddress addr;

    if ((*List = CHAddMapping(*List, UINT64_MAX, 0x1000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
        return EFI_OUT_OF_RESOURCES;
    }

    UInt64 *pd = *Out = (UInt64*)addr;

    EfiZeroMemory(pd, 0x1000);

    /* Now we can go and map everything (including the jump function), and also create the recursive mapping entry (btw,
     * we REALLY need to put this part of the code in some arch-indepdendent file). */

    if (EFI_ERROR((status = CHMapKernel(pd, List, MmuMap)))) {
        return status;
    }

    pd[511] = addr | MMU_PRESENT | MMU_WRITE;

    return EFI_SUCCESS;
}
