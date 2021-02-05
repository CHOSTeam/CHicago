/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 22:53 BRT
 * Last edited on February 05 of 2021 at 11:38 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static EfiStatus MmuMap(UInt32 *PageDir, CHMapping **List, CHMapping *Entry) {
    if (Entry->Type == CH_MEM_MMU) {
        return EFI_SUCCESS;
    }

    /* Simillar to the arm64 code (and the amd64 code), but we only have two levels to worry about (the PD and the
     * PT). */

    UIntN start = 0, size = Entry->Size, level;

    /* First level is 4MiB, let's map what we can using it. */

s:  level = (UIntN)PageDir;

    while (!((Entry->Virtual + start) & 0x3FFFFF) && size >= 0x400000) {
        ((UInt32*)level)[((Entry->Virtual + start) >> 22) & 0x3FF] = (Entry->Physical + start) | MMU_PRESENT
                                                                                               | MMU_WRITE
                                                                                               | MMU_HUGE;
        start += 0x400000;
        size -= 0x400000;
    }

    if (!size) {
        return EFI_SUCCESS;
    }

    /* We still have memory left to map, let's use 4KiB pages. */

    UInt64 tbl = ((UInt32*)level)[((Entry->Virtual + start) >> 22) & 0x3FF];

    if (!(tbl & MMU_PRESENT)) {
        /* Same things that we do on MmuWalkLevel (on amd64/arm64), but here we only have to do it for one level. */

        EfiPhysicalAddress addr;

        if ((*List = CHAddMapping(*List, UINT32_MAX, 0x1000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
            return EFI_OUT_OF_RESOURCES;
        }

        EfiZeroMemory((Void*)addr, 0x1000);

        ((UInt32*)level)[((Entry->Virtual + start) >> 22) & 0x3FF] = addr | MMU_PRESENT | MMU_WRITE;
        level = ((UInt32*)level)[((Entry->Virtual + start) >> 22) & 0x3FF] & ~0xFFF;
    } else if (tbl & MMU_HUGE) {
        EfiDrawString("The MMU paging structures got corrupted during the initialization process.",
                      5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    } else {
        level = tbl & ~0xFFF;
    }

    while (size) {
        ((UInt32*)level)[((Entry->Virtual + start) >> 12) & 0x3FF] = (Entry->Physical + start) | MMU_PRESENT |
                                                                                                 MMU_WRITE;
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

    /* Last but not least: Create the recursive entry. */

    pd[1023] = (addr & ~0xFFF) | MMU_PRESENT | MMU_WRITE;

    return EFI_SUCCESS;
}
