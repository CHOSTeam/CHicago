/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 28 of 2021, at 09:16 BRT
 * Last edited on February 05 of 2021 at 17:28 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static UInt64 MmuMakeEntry(EfiPhysicalAddress Physical, UInt8 Type) {
    return Physical | MMU_PRESENT | MMU_WRITE | (Type == CH_MEM_KCODE ? 0 : MMU_NO_EXEC);
}

static EfiStatus MmuWalkLevel(UInt64 *Level, CHMapping **List, EfiVirtualAddress Virtual, UInt8 Shift, UInt64 *Out) {
    /* Pretty much one to one copy of the MmuWalkLevel function from the arm64 mmu.c file, but adapted for our MMU
     * flags (of course). */

    UInt64 tbl = Level[(Virtual >> Shift) & 0x1FF];

    if (!(tbl & MMU_PRESENT)) {
        EfiPhysicalAddress addr;

        if ((*List = CHAddMapping(*List, UINT64_MAX, 0x1000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
            return EFI_OUT_OF_RESOURCES;
        }

        EfiZeroMemory((Void*)addr, 0x1000);
        Level[(Virtual >> Shift) & 0x1FF] = addr | MMU_PRESENT | MMU_WRITE;
        *Out = addr;
    } else if (tbl & MMU_HUGE) {
        EfiDrawString("The MMU paging structures got corrupted during the initialization process.",
                      5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    } else {
        *Out = tbl & ~0xFFF;
    }

    return EFI_SUCCESS;
} 

static EfiStatus MmuMap(UInt64 *PageDir, CHMapping **List, CHMapping *Entry) {
    if (Entry->Type == CH_MEM_MMU) {
        return EFI_SUCCESS;
    }

    EfiStatus status;
    UIntN start = 0, size = Entry->Size, level;

    /* Skip out the first levels (512GB and 1GB), as we don't support mapping huge pages with them */

s:  level = (UInt64)PageDir;

    if (EFI_ERROR((status = MmuWalkLevel((UInt64*)level, List, Entry->Virtual + start, 39, &level)))) {
        return status;
    } else if (EFI_ERROR((status = MmuWalkLevel((UInt64*)level, List, Entry->Virtual + start, 30, &level)))) {
        return status;
    }

    /* Now the rest is exactly like arm64 (except for the page flags, though that is handled on MmuMakeEntry). And by
     * that I mean that first we map what we can with 2MiB pages (jumping to the beginning if needed), and then we map
     * using 4KiB pages. */

    while (!((Entry->Virtual + start) & 0x1FFFFF) && size >= 0x200000) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 21) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type) | MMU_HUGE;
        start += 0x200000;
        size -= 0x200000;

        if ((((Entry->Virtual + start) >> 30) & 0x1FF) != (((Entry->Virtual + start - 0x200000) >> 30) & 0x1FF)) {
            goto s;
        }
    }

    if (EFI_ERROR((status = MmuWalkLevel((UInt64*)level, List, Entry->Virtual + start, 21, &level)))) {
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

    CHMapping sent = { ((UIntN)ArchJumpIntoCHicago) & ~0xFFF, ((UIntN)ArchJumpIntoCHicago) & ~0xFFF, 0x1000,
                       CH_MEM_KCODE, Null, Null };

    if (EFI_ERROR((status = MmuMap(pd, List, &sent)))) {
        return status;
    }

    for (CHMapping *ent = *List; ent != Null; ent = ent->Next) {
        if (EFI_ERROR((status = MmuMap(pd, List, ent)))) {
            return status;
        }
    }    

    pd[511] = addr | MMU_PRESENT | MMU_WRITE;

    return EFI_SUCCESS;
}
