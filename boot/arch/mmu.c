/* File author is Ítalo Lima Marconato Matias
 *
 * Created on July 10 of 2021, at 16:56 BRT
 * Last edited on July 18 of 2021 at 14:54 BRT */

static EfiStatus MoveInto(MMU_TYPE **Current, UIntN *Level, Mapping **List, EfiVirtualAddress Virtual,
                          EfiPhysicalAddress Physical, UIntN Size) {
    /* Just iterate through the directory until we reach the dest level. The dest level itself and some other things
     * (like indexes into the table) are obtained using macros (that we expect to be defined before including this
     * file). */

    while (*Level != MMU_DEST_LEVEL(Virtual, Physical, Size)) {
        UIntN i = MMU_INDEX(Virtual, *Level);
        MMU_TYPE *next = Null, entry = (*Current)[i];

        if (!MMU_IS_PRESENT(entry)) {
            EfiPhysicalAddress addr;
            Mapping *list = AddMapping(*List, UINTN_MAX, &addr, 0, 0x1000, 0);

            if (list == Null || !addr) return EFI_OUT_OF_RESOURCES;
            EfiZeroMemory((Void*)addr, 0x1000);
            (*Current)[i] = MMU_MAKE_TABLE(addr, *Level);

            next = (MMU_TYPE*)(addr & ~MMU_ENTRY_MASK(*Level));
            *List = list;
        } else if (MMU_IS_HUGE(entry)) {
            EfiDrawString("The MMU paging structures got corrupted during the initialization process.",
                          5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
            return EFI_UNSUPPORTED;
        } else next = (MMU_TYPE*)(entry & ~MMU_ENTRY_MASK(*Level));

        *Current = next;
        (*Level)++;
    }

    return EFI_SUCCESS;
}

static EfiStatus MapAddress(Void *Directory, Mapping **List, Mapping *Entry) {
    /* Most of the indexing/creating new tables and entries logic is implemented by MoveInto and the arch-specific
     * macros, here we just piece everything together (in a partially optimized way). */

    if (!(Entry->Flags & MAP_VIRT)) return EFI_SUCCESS;

    MMU_TYPE *cur = Directory;
    UIntN size = Entry->Size, start = 0, level = 0, i;
    EfiStatus status = MoveInto(&cur, &level, List, Entry->Virtual, Entry->Physical, size);

    if (EFI_ERROR(status)) return status;

    i = MMU_INDEX(Entry->Virtual, level);

    while (size) {
        if (MMU_NEED_TO_MOVE) {
            if (level = 0, cur = Directory, EFI_ERROR((status = MoveInto(&cur, &level, List, Entry->Virtual + start,
                                                                         Entry->Physical + start, size))))
                return status;
            i = MMU_INDEX(Entry->Virtual + start, level);
        }

        cur[i++] = MMU_MAKE_ENTRY(Entry->Physical + start, Entry->Flags, level);
        start += MMU_ENTRY_SIZE(level);
        size -= MMU_ENTRY_SIZE(level);
    }

    return EFI_SUCCESS;
}

static EfiStatus InitDirectory(Mapping **List, Void **Out) {
    /* Alloc the page dir and convert it into a pointer (for MapAddress). */

    EfiStatus status;
    EfiPhysicalAddress addr;
    Mapping *list = AddMapping(*List, UINTN_MAX, &addr, UINT32_MAX, 0x1000, 0);

    if (list == Null || !addr) return EFI_OUT_OF_RESOURCES;
    *List = list;

    MMU_TYPE *pd = *Out = (MMU_TYPE*)addr;

    /* Clean the pagedir, setup recursive paging, and do all the virt->phys mappings that we can do. */

    EfiZeroMemory(pd, 0x1000);

    for (Mapping *cur = *List; cur != Null; cur = cur->Next)
        if (EFI_ERROR((status = MapAddress(*Out, List, cur)))) return status;

#ifdef MMU_SETUP_RECURSIVE
    MMU_SETUP_RECURSIVE();
#else
    return pd[0x1000 / sizeof(MMU_TYPE) - 1] = (addr & ~MMU_ENTRY_MASK(0)) | MMU_RECURSIVE_FLAGS, EFI_SUCCESS;
#endif
}

EfiStatus ArchInitCHicagoMmu(UInt16, Mapping **List, Void **Out) {
    EfiStatus status;
    Mapping ent = { 0x1000, MAP_KERNEL, MAP_VIRT | MAP_EXEC, (UInt64)ArchJumpIntoCHicago & ~0xFFF,
                    (UInt64)ArchJumpIntoCHicago & ~0xFFF, Null, Null };

    if (List == Null || *List == Null || Out == Null) return EFI_INVALID_PARAMETER;
    else if (!ArchGetFeatures(MenuEntryCHicago)) return EFI_UNSUPPORTED;
    else if (EFI_ERROR((status = InitDirectory(List, Out)))) return status;
    else if (EFI_ERROR((status = MapAddress(*Out, List, &ent)))) return status;

    return EFI_SUCCESS;
}
