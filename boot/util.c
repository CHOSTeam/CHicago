/* File author is Ítalo Lima Marconato Matias
 *
 * Created on July 04 of 2021, at 12:25 BRT
 * Last edited on July 06 of 2021 at 18:54 BRT */

#include <efi/lib.h>
#include <util.h>

static Boolean CheckOverlap(UInt64 Pos11, UInt64 Pos12, UInt64 Pos21, UInt64 Pos22) {
    /* Swap the pos*1 and pos*2 if required (pos*1 should be lower than pos*2). */

    if (Pos11 > Pos12) {
        UInt64 tmp = Pos11;
        Pos11 = Pos12;
        Pos12 = tmp;
    }

    if (Pos21 > Pos22) {
        UInt64 tmp = Pos21;
        Pos21 = Pos22;
        Pos22 = tmp;
    }

    /* Now swap the pairs with each other if the starting position of the second pair is lower (just to normalize the
     * overlap check). */

    if (Pos11 > Pos21) {
        UInt64 tmp1 = Pos21, tmp2 = Pos22;
        Pos21 = Pos11;
        Pos22 = Pos12;
        Pos11 = tmp1;
        Pos12 = tmp2;
    }

    return Pos11 <= Pos21 && Pos12 > Pos21;
}

static Mapping *InsertMapping(Mapping *List, EfiVirtualAddress Virtual, EfiPhysicalAddress Physical, UIntN Size,
                              UInt8 Type, UInt8 Flags) {
    /* First entry needs no special checking (only InitMappings() should need this tho). */

    if (List == Null) {
        if ((List = EfiAllocateZeroPool(sizeof(Mapping))) == Null) {
            EfiDrawString("The system is out of memory (couldn't alloc a list entry).", 5, EfiFont.Height + 15,
                          0xFF, 0xFF, 0xFF);
            return Null;
        }

        List->Size = Size;
        List->Type = Type;
        List->Flags = Flags;
        List->Virtual = Virtual;
        List->Physical = Physical;

        return List;
    }

    for (Mapping *cur = List; cur != Null; cur = cur->Next) {
        /* Maybe we should make sure to only error on duplicated mapping entries instead of also memory map entries? */
        if (((Flags & MAP_VIRT) && (cur->Flags & MAP_VIRT) &&
            CheckOverlap(cur->Virtual, cur->Virtual + cur->Size, Virtual, Virtual + Size)) ||
            CheckOverlap(cur->Physical, cur->Physical + cur->Size, Physical, Physical + Size)) {
            EfiDrawString("The kernel/bootloader requested an overlapping allocation.", 5, EfiFont.Height + 15, 0xFF,
                          0xFF, 0xFF);
            return Null;
        } else if ((((Flags & MAP_VIRT) && cur->Virtual + cur->Size == Virtual) || !(Flags & MAP_VIRT)) &&
                   cur->Physical + cur->Size == Physical && cur->Type == Type && cur->Flags == Flags) {
            cur->Size += Size;
            return List;
        } else if ((((Flags & MAP_VIRT) && cur->Virtual == Virtual + Size) || !(Flags & MAP_VIRT)) &&
                   cur->Physical == Physical + Size && cur->Type == Type && cur->Flags == Flags) {
            cur->Size += Size;
            cur->Virtual = Virtual;
            cur->Physical = Physical;
            return List;
        }
    }

    Mapping *ent = EfiAllocateZeroPool(sizeof(Mapping)), *cur = List;

    if (ent == Null) {
        EfiDrawString("The system is out of memory (couldn't alloc a list entry).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        return Null;
    }

    ent->Size = Size;
    ent->Type = Type;
    ent->Flags = Flags;
    ent->Virtual = Virtual;
    ent->Physical = Physical;

    if (List->Physical > Physical) {
        List->Prev = ent;
        ent->Next = List;
        return ent;
    }

    /* Maintain the list sorted (so make sure to find the right place, and put ourselves in the right position
     * (relative to the found entry). */

    for (; cur->Next != Null && cur->Physical < Physical; cur = cur->Next) ;

    if (cur->Physical > Physical) {
        ent->Prev = cur->Prev;
        ent->Prev->Next = ent;
        ent->Next = cur;
        cur->Prev = ent;
    } else {
        if (cur->Next != Null) {
            ent->Next = cur->Next;
            ent->Next->Prev = ent;
        }

        ent->Prev = cur;
        cur->Next = ent;
    }

    return List;
}

Mapping *AddMapping(Mapping *List, EfiVirtualAddress Virtual, EfiPhysicalAddress *Physical, UIntN Size, UInt8 Flags) {
    UIntN pages = (Size + 0xFFF) >> 12, size = pages << 12;

    if (((Flags & MAP_VIRT) && !(Virtual &= ~0xFFF)) || Physical == Null || !Size) return Null;
    else if (!(Flags & MAP_DEVICE) && !(*Physical = EfiAllocatePages(pages))) {
        EfiDrawString("The system is out of memory (couldn't alloc physical memory).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        return Null;
    }

    /* We need to remember to block any duplicate mappings, and also, we need to remove/split any free/res entries that
     * contains the region we're mapping. */

    Boolean adv = True;

    for (Mapping *cur = List; cur != Null; cur = adv ? cur->Next : cur, adv = True) {
        if (CheckOverlap(cur->Physical, cur->Physical + cur->Size, *Physical, *Physical + size)) {
            if (cur->Type == MAP_KERNEL) continue;
            if (*Physical <= cur->Physical && *Physical + size >= cur->Physical + cur->Size) {
                /* This whole entry is part of the new mapping, no need to shave off just a specific part, instead,
                 * completely remove it from the list. */

                Mapping *old = cur;
                cur = cur->Next;
                adv = False;

                if (old->Next != Null) old->Next->Prev = old->Prev;
                if (old->Prev != Null) old->Prev->Next = old->Next;
                else List = Null;

                EfiFreePool(old);
            } else if (*Physical <= cur->Physical && *Physical + size < cur->Physical + cur->Size) {
                cur->Size = cur->Physical + cur->Size - *Physical - size;
                cur->Physical = *Physical + size;
            } else if (*Physical > cur->Physical && *Physical + size >= cur->Physical + cur->Size)
                cur->Size = *Physical - cur->Physical;
            else {
                /* This is the more complex case of the bunch: the new entry is in the middle of this one, so we need
                 * to split this entry in two, while fixing the physical address and size of each entry. */

                Mapping *ent = EfiAllocateZeroPool(sizeof(Mapping));

                if (ent == Null) {
                    EfiDrawString("The system is out of memory (couldn't alloc a list entry).", 5, EfiFont.Height + 15,
                                  0xFF, 0xFF, 0xFF);
                    return Null;
                }

                ent->Size = cur->Physical + cur->Size - *Physical - size;
                ent->Type = cur->Type;
                ent->Flags = cur->Flags;
                ent->Virtual = UINT64_MAX;
                ent->Physical = *Physical + Size;
                ent->Prev = cur;
                ent->Next = cur->Next;
                if (cur->Next != Null) cur->Next->Prev = ent;

                cur->Size = *Physical - cur->Physical;
                cur->Next = ent;
            }
        }
    }

    Mapping *res = InsertMapping(List, Virtual, *Physical, size, MAP_KERNEL, Flags);
    if (res == Null && !(Flags & MAP_DEVICE)) EfiFreePages(*Physical, pages);
    return res;
}

Mapping *InitMappings(Void) {
    Mapping *list = Null;

    /* We only need to do one thing here: grab the memory map and initialize our list using all it's entries, fixing
     * any "duplicate" entries (entries that continue some other one or that are really duplicated) and really just
     * marking everything correctly. */

    UInt32 dver = 0;
    UIntN dsize = 0, mcount = 0, mkey = 0;
    EfiMemoryDescriptor *map = EfiGetMemoryMap(&mcount, &mkey, &dsize, &dver);

    if (map == Null) {
        EfiDrawString("Couldn't get the memory map (the system may be out of memory).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        return Null;
    }

    for (UIntN i = 0; i < mcount; i++) {
        EfiMemoryDescriptor *desc = (EfiMemoryDescriptor*)((UIntN)map + i * dsize);
        Mapping *res = InsertMapping(list, UINT64_MAX, desc->PhysicalStart, desc->NumberOfPages << 12,
                                     desc->Type == EfiBootServicesData || desc->Type == EfiBootServicesCode ||
                                     desc->Type == EfiRuntimeServicesData || desc->Type == EfiRuntimeServicesCode ||
                                     desc->Type == EfiLoaderData || desc->Type == EfiLoaderCode ||
                                     desc->Type == EfiConventionalMemory ? MAP_FREE : MAP_RES, 0);
        if (res == Null) {
            while (list != Null) {
                Mapping *cur = list;
                list = list->Next;
                EfiFreePool(cur);
            }

            return Null;
        }

        list = res;
    }

    return list;
}
