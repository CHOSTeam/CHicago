/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 12:46 BRT
 * Last edited on February 04 of 2021 at 13:06 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static UIntN MmuHigherHalfStart = 0xFFFFFF8000000000, MmuHigherHalfEnd = 0xFFFFFFFF80000000;
static Boolean MmuSupports48 = False;

static UInt64 MmuMakeEntry(EfiPhysicalAddress Physical, UInt8 Type) {
    UInt64 base = Physical | MMU_PRESENT | MMU_INNER_SHARE | MMU_ACCESS;

    switch (Type) {
    case CH_MEM_KCODE: {
        return base;
    }
    case CH_MEM_DEV: {
        return base | MMU_DEVICE | MMU_NO_EXEC;
    }
    default: {
        return base | MMU_NO_EXEC;
    }
    }
}

static EfiStatus MmuWalkLevel(UInt64 *Level, CHMapping **List, EfiVirtualAddress Virtual, UInt8 Shift, UIntN *Out) {
    UInt64 tbl = Level[(Virtual >> Shift) & 0x1FF];

    if (!(tbl & MMU_PRESENT)) {
        /* Allocate/reserve the memory for the table entry. */

        EfiPhysicalAddress addr;

        if ((*List = CHAddMapping(*List, UINT64_MAX, 0x1000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
            return EFI_OUT_OF_RESOURCES;
        }
    
        EfiZeroMemory((Void*)addr, 0x1000);
        
        Level[(Virtual >> Shift) & 0x1FF] = addr | MMU_PRESENT | MMU_TABLE;
        tbl = Level[(Virtual >> Shift) & 0x1FF];
    } else if (!(tbl & MMU_TABLE)) {
        EfiDrawString("The MMU paging structures got corrupted during the initialization process.",
                      5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    }

    *Out = tbl & ~0xFFF;

    return EFI_SUCCESS;
}

static EfiStatus MmuMap(UInt64 **PageDir, CHMapping **List, CHMapping *Entry) {
    UInt8 high = 0;

    if (Entry->Type == CH_MEM_MMU) {
        /* MMU entries (like the page directories or page tables) should not be mapped into virtual memory (as it will
         * be mapped by recursive paging). */

        return EFI_SUCCESS;
    } else if (Entry->Virtual >= MmuHigherHalfStart) {
        /* Arm64 divides the address space in two (that is, uses two different page directories for the address
         * space), the first one is for the lower half, and the second one, as you may imagine, for the higher half.
         * And we need to handle when to use each one. */
    
        high++;
    }

    /* Save the size, as we may use pages of different sizes to map everything (and so we will decrease the size as we
     * map using bigger pages). */

    EfiStatus status;
    UIntN start = 0, size = Entry->Size, level;

    /* If we have 48-bits support, we need to skip/alloc the first level, as we're not going to handle checking if we
     * support 512GB pages. */

s:  level = (UIntN)PageDir[high];

    if (MmuSupports48 && EFI_ERROR((status = MmuWalkLevel((UInt64*)level, List, Entry->Virtual + start, 39,
                                                          &level)))) {
        return status;
    }

    /* We're going for now assume that 1GiB pages ARE supported, so let's first map things using said huge pages. */

    while (!((Entry->Virtual + start) & 0x3FFFFFFF) && size >= 0x40000000) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 30) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type);
        start += 0x40000000;
        size -= 0x40000000;

        /* Check if we haven't exceeded the limit of this 512GB entry (if we are on 48-bits, of course). */

        if (MmuSupports48 && (((Entry->Virtual + start) >> 39) & 0x1FF) !=
                              (((Entry->Virtual + start - 0x40000000) >> 39) & 0x1FF)) {
            goto s;
        }
    }

    if (!size) {
        return EFI_SUCCESS;
    }

    /* We still have pages left to map, let's walk through into the next/first level. */

    if (EFI_ERROR((status = MmuWalkLevel((UInt64*)level, List, Entry->Virtual + start, 30, &level)))) {
        return status;
    }

    /* And now let's map what we can/need using 2MiB pages. */

    while (!((Entry->Virtual + start) & 0x1FFFFF) && size >= 0x200000) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 21) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type);
        start += 0x200000;
        size -= 0x200000;

        if ((((Entry->Virtual + start) >> 30) & 0x1FF) != (((Entry->Virtual + start - 0x200000) >> 30) & 0x1FF)) {
            goto s;
        }
    }

    /* Finally, last level walk, as now we gonna reach the 4KiB mappings. */

    if (EFI_ERROR((status = MmuWalkLevel((UInt64*)level, List, Entry->Virtual + start, 21, &level)))) {
        return status;
    }

    while (size) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 12) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type) | MMU_TABLE;
        start += 0x1000;
        size -= 0x1000;

        if ((((Entry->Virtual + start) >> 21) & 0x1FF) != (((Entry->Virtual + start - 0x1000) >> 21) & 0x1FF)) {
            goto s;
        }
    }

    return EFI_SUCCESS;
}

EfiStatus ArchInitCHicagoMmu(UInt16 Features, CHMapping **List, Void **Out) {
    if (List == Null || *List == Null || Out == Null) {
        return EFI_INVALID_PARAMETER;
    }

    /* First, start by gattering if we can use 48-bits addressing (and so what is going be the start of the higher
     * half), and if the required granularities are supported. */

    if (!ArchGetFeatures(MenuEntryCHicago)) {
        EfiDrawString("The system MMU is not supported.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    } else if (Features & SIA_ARM64_48_BITS) {
        MmuHigherHalfStart = 0xFFFF000000000000;
        MmuHigherHalfEnd = 0xFFFFFF0000000000;
        MmuSupports48 = True;
    }

    /* Reserve/allocate the paging addresses, and convert the addresses into pointers (as our map function takes two
     * pointers). */

    EfiStatus status;
    EfiPhysicalAddress addr;

    if ((*List = CHAddMapping(*List, UINT64_MAX, 0x2000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
        return EFI_OUT_OF_RESOURCES;
    }

    UInt64 **pdp = *Out = EfiAllocatePool(sizeof(UInt64*) * 2);

    if (Out == Null) {
        EfiDrawString("The system is out of memory (couldn't allocate the page directory pointer).", 5,
                      EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_OUT_OF_RESOURCES;
    }
    
    pdp[0] = (UInt64*)addr;
    pdp[1] = (UInt64*)(addr + 0x1000);

    /* We need to clean the pointers, else, we may have some non zeroed memory that may make the mapping process fail
     * or crash. */

    EfiZeroMemory(pdp[0], 0x1000);
    EfiZeroMemory(pdp[1], 0x1000);

    CHMapping sent = { ((UIntN)ArchJumpIntoCHicago) & ~0xFFF, ((UIntN)ArchJumpIntoCHicago) & ~0xFFF, 0x1000,
                       CH_MEM_KCODE, Null, Null };

    if (EFI_ERROR((status = MmuMap(pdp, List, &sent)))) {
        return status;
    }

    for (CHMapping *ent = *List; ent != Null; ent = ent->Next) {
        if (EFI_ERROR((status = MmuMap(pdp, List, ent)))) {
            return status;
        }
    }

    /* Create the recursive entries. */

    pdp[1][510] = (addr & ~0xFFF) | MMU_PRESENT | MMU_TABLE;
    pdp[1][511] = ((addr + 0x1000) & ~0xFFF) | MMU_PRESENT | MMU_TABLE;

    return EFI_SUCCESS;
}
