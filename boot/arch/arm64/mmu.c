/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 12:46 BRT
 * Last edited on February 12 of 2021 at 12:08 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static inline UInt64 MmuMakeEntry(EfiPhysicalAddress Physical, UInt8 Type) {
    return Physical | MMU_PRESENT | MMU_INNER_SHARE | MMU_ACCESS | (Type != CH_MEM_KCODE ? MMU_NO_EXEC : 0) |
                      ((Type == CH_MEM_KCODE || Type == CH_MEM_KDATA_RO) ? MMU_READ_ONLY : 0) |
                      (Type == CH_MEM_DEV ? MMU_DEVICE : 0);
}

static Boolean MmuIsPresent(UIntN Table) {
    return Table & MMU_PRESENT;
}

static Boolean MmuIsHuge(UIntN Table) {
    return !(Table & MMU_TABLE);
}

static EfiStatus MmuMap(Void *Directory, CHMapping **List, CHMapping *Entry) {
    UInt64 **pd = Directory;
    UInt8 high = 0;

    if (Entry->Type == CH_MEM_MMU) {
        /* MMU entries (like the page directories or page tables) should not be mapped into virtual memory (as it will
         * be mapped by recursive paging). */

        return EFI_SUCCESS;
    } else if (Entry->Virtual >= 0xFFFF000000000000) {
        /* Arm64 divides the address space in two (that is, uses two different page directories for the address
         * space), the first one is for the lower half, and the second one, as you may imagine, for the higher half.
         * And we need to handle when to use each one. */
    
        high++;
    }

    /* Save the size, as we may use pages of different sizes to map everything (and so we will decrease the size as we
     * map using bigger pages). */

    EfiStatus status;
    UIntN start = 0, size = Entry->Size, level;

    /* Skip/alloc the first two level, as we're not going to map using 512GB and 1GB pages (yet). */

s:  level = (UInt64)pd[high];

    if (EFI_ERROR((status = CHWalkMmuLevel((UInt64*)level, List, Entry->Virtual + start, 39, 0x1FF,
                                           MMU_PRESENT | MMU_TABLE | MMU_INNER_SHARE | MMU_ACCESS,
                                           MmuIsPresent, MmuIsHuge, &level)))) {
        return status;
    } else if (EFI_ERROR((status = CHWalkMmuLevel((UInt64*)level, List, Entry->Virtual + start, 30, 0x1FF,
                                                  MMU_PRESENT | MMU_TABLE | MMU_INNER_SHARE | MMU_ACCESS,
                                                  MmuIsPresent, MmuIsHuge, &level)))) {
        return status;
    }

    /* And now let's map what we can/need using 2MiB pages. */

    while (!(((Entry->Virtual + start) & 0x1FFFFF) || ((Entry->Physical + start) & 0x1FFFFF)) && size >= 0x200000) {
        ((UInt64*)level)[((Entry->Virtual + start) >> 21) & 0x1FF] = MmuMakeEntry(Entry->Physical + start,
                                                                                  Entry->Type);
        start += 0x200000;
        size -= 0x200000;

        if ((((Entry->Virtual + start) >> 30) & 0x1FF) != (((Entry->Virtual + start - 0x200000) >> 30) & 0x1FF)) {
            goto s;
        }
    }

    /* Finally, last level walk, as now we gonna reach the 4KiB mappings. */

    if (EFI_ERROR((status = CHWalkMmuLevel((UInt64*)level, List, Entry->Virtual + start, 21, 0x1FF,
                                           MMU_PRESENT | MMU_TABLE | MMU_INNER_SHARE | MMU_ACCESS,
                                           MmuIsPresent, MmuIsHuge, &level)))) {
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

EfiStatus ArchInitCHicagoMmu(UInt16, CHMapping **List, Void **Out) {
    if (List == Null || *List == Null || Out == Null) {
        return EFI_INVALID_PARAMETER;
    }

    /* First, start by calling GetFeatures (so that we know if 4KiB pages are supported). */

    if (!ArchGetFeatures(MenuEntryCHicago)) {
        EfiDrawString("The MMU doesn't support 4KiB granularity.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    }

    /* Reserve/allocate the paging addresses, and convert the addresses into pointers (as our map function takes two
     * pointers). */

    EfiStatus status;
    EfiPhysicalAddress addr;

    if ((*List = CHAddMapping(*List, UINT64_MAX, 0x2000, CH_MEM_MMU, &addr, True)) == Null || !addr) {
        return EFI_OUT_OF_RESOURCES;
    }

    UInt64 **pd = *Out = EfiAllocatePool(sizeof(UInt64*) * 2);

    if (Out == Null) {
        EfiDrawString("The system is out of memory (couldn't allocate the page directory pointer).", 5,
                      EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_OUT_OF_RESOURCES;
    }
    
    pd[0] = (UInt64*)addr;
    pd[1] = (UInt64*)(addr + 0x1000);

    /* We need to clean the pointers, else, we may have some non zeroed memory that may make the mapping process fail
     * or crash. */

    EfiZeroMemory(pd[0], 0x2000);

    if (EFI_ERROR((status = CHMapKernel(pd, List, MmuMap)))) {
        return status;
    }

    /* Create the recursive entries. */

    pd[1][510] = addr | MMU_PRESENT | MMU_TABLE | MMU_INNER_SHARE | MMU_ACCESS;
    pd[1][511] = (addr + 0x1000) | MMU_PRESENT | MMU_TABLE | MMU_INNER_SHARE | MMU_ACCESS;

    return EFI_SUCCESS;
}
