/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 16:41 BRT
 * Last edited on February 06 of 2021 at 17:27 BRT */

#include <arch.h>
#include <efi/lib.h>
#include <elf.h>

/* TODO: Is there any way to alloc pages (using AllocatePages) while using a custom memory type (that doesn't break
 * under older firmwares)? If there is, we can simplify the CHAddMapping function to not use the CHMapping struct
 * (and also simplify some other parts of the code). */

CHMapping *CHAddMapping(CHMapping *List, EfiVirtualAddress Virtual, UIntN Size, UInt8 Type,
                        EfiPhysicalAddress *Physical, Boolean Allocate) {
    /* CHicago is not mapped on a continuous and pre-determinated range of physical addresses (anymore), so we need to
     * actually allocate the physical memory (using the EfiAllocatePages function), and handle everything so that
     * regions that are continuous on both physical and virtual memory will be grouped together (on the same entry).
     * The mapping entries each have both a pointer to the next mapping and to the previous one, so that we can add
     * and remove entries. */

    UIntN pages = (Size + 0xFFF) >> 12, size = pages << 12;

    if (Physical == Null) {
        return Null;
    } else if (Allocate) {
        *Physical = 0;
    }

    if (List == Null) {
        /* First entry, we can just allocate the memory for the first entry struct, allocate the physical address (if
         * required), and return. */

        List = EfiAllocateZeroPool(sizeof(CHMapping));

        if (List == Null) {
            EfiDrawString("The system is out of memory (couldn't alloc a list entry).", 5, EfiFont.Height + 15, 0xFF,
                          0xFF, 0xFF);
            *Physical = 0;
            return Null;
        } else if (Allocate && !(*Physical = EfiAllocatePages(pages))) {
            EfiDrawString("The system is out of memory (couldn't alloc physical memory).", 5, EfiFont.Height + 15,
                          0xFF, 0xFF, 0xFF);
            EfiFreePool(List);
            return Null;
        }

        List->Virtual = Virtual;
        List->Physical = *Physical;
        List->Size = size;
        List->Type = Type;

        return List;
    }

    /* First, check if the virtual address is maybe just after some other address (which could consist in a whole
     * continuous region). */

    CHMapping *i = List;

    for (; i != Null; i = i->Next) {
        if ((Type != CH_MEM_MMU && i->Type != CH_MEM_MMU &&
             (i->Virtual + i->Size == Virtual || Virtual + size == i->Virtual)) ||
            (Type == CH_MEM_MMU && i->Type == CH_MEM_MMU)) {
            break;
        } else if (!(i->Type == CH_MEM_MMU || Type == CH_MEM_MMU) &&
                   ((Virtual >= i->Virtual && Virtual < i->Virtual + i->Size) ||
                   (Virtual + size >= i->Virtual && Virtual + size < i->Virtual + i->Size))) {
            /* The requested virtual address falls just in the middle of another entry. So let's error out. */

            EfiDrawString("The kernel/bootloader requested a overlapping allocation.", 5, EfiFont.Height + 15, 0xFF,
                          0xFF, 0xFF);

            *Physical = 0;

            return List;
        }
    }

    /* The merging/checking if maybe multiple pages that already exists form a unique whole region will be made post
     * creation of the entry. What we have to do now is allocate the physical address, so that in case 'i' isn't
     * Null (that is, this entry could be a region), we can just increase the size (and possibly decrease the starting
     * virt of the region). */

    if (Allocate && !(*Physical = EfiAllocatePages(pages))) {
        /* By the way, if may not have been obvious on the first entry case, but we always return the starting point
         * of the list, and if the allocation fails, the Physical parameter (which is a pointer) will indicate that we
         * failed (it will be 0, instead of the allocated physical address). */

        EfiDrawString("The system is out of memory (couldn't alloc physical memory).", 5, EfiFont.Height + 15, 0xFF,
                      0xFF, 0xFF);

        return List;
    } else if (i != Null && i->Type == Type && (i->Type == CH_MEM_MMU || i->Virtual + i->Size == Virtual) &&
               i->Physical + i->Size == *Physical) {
        /* Continuous region, where the newly allocated physical address, and the virtual address that we were asked
         * to add/map, are just after one of the entries (so we just need to increase the size). */

        i->Size += Size;

        return List;
    }

    /* Not continuous entry (at least physically, or maybe because of the type), let's now alloc the new entry (as we
     * probably need it). */

    CHMapping *ent = EfiAllocateZeroPool(sizeof(CHMapping));

    if (ent == Null) {
        EfiFreePages(*Physical, pages);
        *Physical = 0;
        return List;
    }

    ent->Virtual = Virtual;
    ent->Physical = *Physical;
    ent->Size = Size;
    ent->Type = Type;

    if (Type == CH_MEM_MMU) {
        /* MMU allocations will come at the end. */

        for (i = List; i->Next != Null; i = i->Next) ;
    } else if (i == Null && List->Virtual > Virtual) {
        /* We need to take over the first entry (as we come before it). */

        i = List;
    } else if (i == Null) {
        /* The loop did not find any entry, and we're not the new first entry. In this case, we need to find the
         * first entry that fits the criteria i->Virtual < Virtual (we are higher on virtual memory than them). */

        for (i = List; i->Virtual > Virtual && i->Next != Null; i = i->Next) ;
    }

    if (i->Virtual <= Virtual) {
        if (i->Next != Null) {
            ent->Next = i->Next;
            ent->Next->Prev = ent;
        }

        ent->Prev = i;
        i->Next = ent;
    } else {
        if (i->Prev != Null) {
            ent->Prev = i->Prev;
            ent->Prev->Next = ent;
        }

        ent->Next = i;
        i->Prev = ent;
    }

    if (ent->Next == List) {
        /* We're now the first entry. */

        List = ent;
    }

    /* Now while we did add the entry in the right place, we need to colapse/merge together continuous entries. */

     for (i = List; i != Null && i->Next != Null; i = i->Next) {
        while (i->Next != Null && i->Type == i->Next->Type && i->Type != CH_MEM_DEV && i->Type != CH_MEM_MMU && 
               i->Virtual + i->Size == i->Next->Virtual && i->Physical + i->Size == i->Next->Physical) {
            ent = i->Next;

            i->Size += ent->Size;
            i->Next = ent->Next;

            if (i->Next != Null) {
                i->Next->Prev = i;
            }

            EfiFreePool(ent);
        }
    }

    return List;
}

static CHMapping *CHGetMapping(CHMapping *List, UIntN Start, UIntN End) {
    for (; List != Null; List = List->Next) {
        if ((Start <= List->Physical && End > List->Physical && End < (List->Physical + List->Size)) ||
            (Start >= List->Physical && Start < (List->Physical + List->Size))) {
            break;
        }
    }

    return List;
}

static EfiStatus SiaCheck(UInt8 *Buffer, UIntN Size) {
    /* SIA (System Image Archive) is a TAR-like file format, used here on the osldr as the kernel/initrd container. It
     * can contain kernel images for multiple "variants" of the same architecture (for example, one for arm64 with 48-bit
     * VA support, and one for arm64 with 39-bit VA support), and also multiple root images/initrds. */

    if (Size < sizeof(SiaHeader)) {
        EfiDrawString("The boot image file size is too small.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    }

    SiaHeader *hdr = (SiaHeader*)Buffer;

    if (hdr->Magic != SIA_MAGIC) {
        EfiDrawString("The boot image file magic is invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    } else if (hdr->FreeFileCount && (hdr->FreeFileOffset < sizeof(SiaHeader) ||
               hdr->FreeFileOffset + hdr->FreeFileCount * sizeof(SiaFile) >= Size)) {
        EfiDrawString("The free file count/offset is invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    } else if (hdr->FreeDataCount && (hdr->FreeDataOffset < sizeof(SiaHeader) ||
               hdr->FreeDataOffset + hdr->FreeDataCount * sizeof(SiaData) >= Size)) {
        EfiDrawString("The free data count/offset is invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    }

    /* Check each kernel and root image, as there can be up to 16 (at the time of writing this comment) of them. */

    for (UIntN i = 0; i < sizeof(hdr->KernelImages) / sizeof(UInt64); i++) {
        if (hdr->KernelImages[i] && (hdr->KernelImages[i] < sizeof(SiaHeader) ||
                                     hdr->KernelImages[i] + sizeof(SiaFile) >= Size)) {
            EfiDrawString("One of the kernel file offsets are invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
            return EFI_INVALID_PARAMETER;
        }
    }

    for (UIntN i = 0; i < sizeof(hdr->RootImages) / sizeof(UInt64); i++) {
        if (hdr->RootImages[i] && (hdr->RootImages[i] < sizeof(SiaHeader) ||
                                   hdr->RootImages[i] + sizeof(SiaFile) >= Size)) {
            EfiDrawString("One of the root image offsets are invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
            return EFI_INVALID_PARAMETER;
        }
    }

    return EFI_SUCCESS;
}

static EfiStatus SiaGoToOffset(UInt8 *SourceAddress, UIntN SourceSize, SiaFile *File, UInt64 Offset, UInt64 *Last) {
    /* By default, init the output argument (Last) with the offset of the start of the file. */

    SiaData *data = Null;

    *Last = File->Offset;

    /* Now, let's follow all the links until we find the SiaData struct that we want. */

    while (Offset >= sizeof(data->Data)) {
        if (*Last >= SourceSize || !*Last) {
            return EFI_BUFFER_TOO_SMALL;
        }

        data = (SiaData*)&SourceAddress[*Last];
        Offset -= sizeof(data->Data);
        *Last = data->Next;
    }

    return EFI_SUCCESS;
}

static EfiStatus SiaReadFile(UInt8 *SourceAddress, UIntN SourceSize, SiaFile *File, UInt64 Offset, UInt64 Length,
                             UInt8 *OutAddress) {
    SiaData *data;
    UInt64 last = 0, cur = 0, skip = Offset % sizeof(data->Data);

	/* First, we need to get the location where this file starts, it could be as simple as just using File->Offset, but
	 * we need to account for the Offset argument. */

	EfiStatus status = SiaGoToOffset(SourceAddress, SourceSize, File, Offset, &last);

	if (EFI_ERROR(status)) {
	    return status;
	}

	/* Now we can just follow all the links in the file struct, while copying all the data. */

	while (Length) {
	    if (last >= SourceSize || !last) {
	        return EFI_BUFFER_TOO_SMALL;
	    }

	    data = (SiaData*)&SourceAddress[last];

	    UInt64 size = sizeof(data->Data) - skip;

	    if (size > Length) {
	        size = Length;
	    }

	    EfiCopyMemory(&OutAddress[cur], &data->Data[skip], size);

	    skip = 0;
	    cur += size;
	    Length -= size;
	    last = data->Next;
	}

	return EFI_SUCCESS;
}

static EfiStatus SiaLoadKernel(UInt8 *Buffer, UIntN Size, UInt16 *Features, UIntN *Entry, UIntN *Start, UIntN *End,
                               CHMapping **List) {
    /* First, check if the SIA file is valid and not corrupted. */

    EfiStatus status = SiaCheck(Buffer, Size);

    if (EFI_ERROR(status)) {
        return status;
    }

    /* Now get the optimal kernel image (we can have multiple kernel images, so we need to do this). */

    SiaHeader *hdr = (SiaHeader*)Buffer;
    SiaFile *file = ArchGetBestFitCHicago(hdr, Size, Features);

    if (file == Null) {
        EfiDrawString("Couldn't find a compatible kernel image.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    }

    /* Now we can read the ELF header, and make sure the file is actually a valid kernel for this architeture. */

    ElfHeader ehdr;

    if (EFI_ERROR((status = SiaReadFile(Buffer, Size, file, 0, sizeof(ElfHeader), (UInt8*)&ehdr)))) {
        EfiDrawString("Couldn't read the kernel ELF header.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return status;
    } else if (ehdr.Ident[0] != 0x7F || ehdr.Ident[1] != 'E' || ehdr.Ident[2] != 'L' || ehdr.Ident[3] != 'F') {
        EfiDrawString("The kernel image is not a valid ELF file", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    } else if (ehdr.Type != 2) {
        EfiDrawString("The ELF file is not suited to be a kernel image.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    } else if (ehdr.Machine != ELF_MACHINE) {
        EfiDrawString("The kernel image is not for this architecture.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    } else if (!ehdr.Version) {
        EfiDrawString("The kernel ELF header version field is invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_INVALID_PARAMETER;
    }

    /* Now we need to load all the sections into memory, and for that, we need the ELF program headers. Let's allocate
     * some space and load them. */

    ElfProgHeader *phdrs = EfiAllocatePool(ehdr.ProgHeaderCount * sizeof(ElfProgHeader));

    if (phdrs == Null) {
        EfiDrawString("The system is out of memory (couldn't alloc memory for reading the program headers).", 5,
                      EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_OUT_OF_RESOURCES;
    } else if ((EFI_ERROR((status = SiaReadFile(Buffer, Size, file, ehdr.ProgHeaderOffset,
                                                ehdr.ProgHeaderCount * sizeof(ElfProgHeader), (UInt8*)phdrs))))) {
        EfiDrawString("Couldn't read the kernel ELF program headers.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        EfiFreePool(phdrs);
        return status;
    }

    /* The sections all have both the virtual address (as we're supposed to jump into the kernel after enabling
     * paging) and the physical address, we need to load everything into the physical address space. */

    *Start = UINTN_MAX;
    *End = 0;

    for (UIntN i = 0; i < ehdr.ProgHeaderCount; i++) {
        /* And here we are fixing 0x1000 as the page size again lol. */

        EfiPhysicalAddress addr;
        ElfProgHeader *phdr = &phdrs[i];
        UIntN size = (phdr->MemSize + 0xFFF) & ~0xFFF;

        /* The size in memory and size in the file may be different, so let's clean it first, and load what we need
         * into memory after that. */

        if ((*List = CHAddMapping(*List, phdr->VirtAddress, size, (phdr->Flags & 0x01) ? CH_MEM_KCODE :
                                                                  ((phdr->Flags & 0x02) ? CH_MEM_KDATA :
                                                                                          CH_MEM_KDATA_RO),
                                  &addr, True)) == Null || !addr) {
            EfiFreePool(phdrs);
            return status;
        }

        if (phdr->VirtAddress < *Start) {
            *Start = phdr->VirtAddress;
        }

        if (phdr->VirtAddress + size > *End) {
            *End = phdr->VirtAddress + size;
        }
        
        EfiZeroMemory((Void*)addr, size);

        if (phdr->FileSize && EFI_ERROR((status = SiaReadFile(Buffer, Size, file, phdr->Offset, phdr->FileSize,
                                                              (UInt8*)addr)))) {
            EfiDrawString("Couldn't read one of the kernel sections.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
            EfiFreePool(phdrs);
            return status;
        }
    }

    /* All done now, free the memory we allocated for reading the prog headers, and save the entry point. */

    EfiFreePool(phdrs);

    *Entry = ehdr.Entry;

    return EFI_SUCCESS;
}

EfiStatus LdrStartCHicago(MenuEntry *Entry) {
    /* Entry should be valid, but let's make sure. */

    if (Entry == Null || Entry->Path == Null || Entry->Type != MenuEntryCHicago) {
        return EFI_INVALID_PARAMETER;
    }

    /* Let's read in the SIA file (in whatever path it is), validate it, and load the best fit kernel into memory
     * (everything but loading in the file into memory is done by SiaLoadKernel). */

    UInt8 *buf;
    UInt16 feat;
    EfiFile *file;
    CHMapping *list = Null;
    UIntN size, entry, start, end;

    EfiStatus status = EfiOpenFile(Entry->Path, EFI_FILE_MODE_READ, &file);

    if (EFI_ERROR(status) || file == Null) {
        EfiDrawString("Failed to open the boot image file.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return status;
    } else if (!(size = EfiGetFileSize(file))) {
        EfiDrawString("Failed to get the boot image file size.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return status;
    } else if ((buf = EfiAllocatePool(size)) == Null) {
        EfiDrawString("The system is out of memory (couldn't allocate memory for reading the boot image).", 5,
                      EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        file->Close(file);
        return status;
    } else if (EFI_ERROR((status = file->Read(file, &size, buf)))) {
        EfiDrawString("Failed to read the boot image file.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        EfiFreePool(buf);
        file->Close(file);
        return status;
    }

    /* The EFI file handle is now useless, we can close it and go into loading the kernel. */

    file->Close(file);

    if (EFI_ERROR((status = SiaLoadKernel(buf, size, &feat, &entry, &start, &end, &list)))) {
        EfiFreePool(buf);
        return status;
    }

    /* CHicago expects the start function to be called while a special boot struct is passed as the first arg, let's
     * reserve the space for said boot struct. */

    UIntN asize = (sizeof(CHBootInfo) + 0xFFF) & ~0xFFF;
    EfiPhysicalAddress addr;
    UIntN biv = (UIntN)end;
    CHBootInfo *bi;

    if ((list = CHAddMapping(list, end, asize, CH_MEM_KDATA, &addr, True)) == Null || !addr) {
        EfiFreePool(buf);
        return EFI_OUT_OF_RESOURCES;
    }

    bi = (CHBootInfo*)addr;
    end += asize;

    EfiZeroMemory(bi, asize);

    /* Relloc the boot SIA image file contents (to a place that the kernel will be able to access post
     * ExitBootServices). */

    UInt8 *nbuf, *nbufv = (UInt8*)end;

    asize = (size + 0xFFF) & ~0xFFF;

    if ((list = CHAddMapping(list, end, asize, CH_MEM_KDATA, &addr, True)) == Null || !addr) {
        EfiFreePool(buf);
        return EFI_OUT_OF_RESOURCES;
    }

    nbuf = (UInt8*)addr;
    end += asize;

    /* Zero the memory as we probably over allocated (pretty hard to not happen). */

    EfiZeroMemory(nbuf, asize);
    EfiCopyMemory(nbuf, buf, size);
    EfiFreePool(buf);

    /* Map the framebuffer into virtual memory. */

    UIntN fbv = (UIntN)end;

    addr = EfiGop->Mode->FrameBufferBase;
    asize = (EfiGop->Mode->FrameBufferSize + 0xFFF) & ~0xFFF;

    if ((list = CHAddMapping(list, end, asize, CH_MEM_DEV, &addr, False)) == Null || !addr) {
        return EFI_OUT_OF_RESOURCES;
    }

    end += asize;

    /* Allocate space for the memory map, so that the kernel knows which PHYSICAL memory regions are free. For getting
     * the EFI memory map, we need to call EfiGetMemoryMap (duh). */

    UInt32 dver = 0;
    UInt64 minaddr = 0, maxaddr = 0, maxsize = 0;
    UIntN dsize = 0, mmapc = 0, msize = 0, mcount = 0, mkey = 0;
    EfiMemoryDescriptor *map = EfiGetMemoryMap(&mcount, &mkey, &dsize, &dver);

    if (map == Null) {
        EfiDrawString("Couldn't get the memory map (the system may be out of memory).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        return EFI_OUT_OF_RESOURCES;
    }

    /* Now we can allocate the kernel's memory map, but we're not going start filling it just yet (as we still have
     * some things to allocate in physical memory). But we do need to get the memory size and max addressable physical
     * address (+ first addressable address). Also, the '* mcount * 2' instead of just '* mcount' is as the mmap may
     * get a bit bigger with the next allocations, and we don't know how big it will get (so we need to be safe). */

    CHBootInfoMemMap *mmap, *mmapv = (CHBootInfoMemMap*)end;

    asize = (sizeof(CHBootInfoMemMap) * mcount * 2 + 0xFFF) & ~0xFFF;

    if ((list = CHAddMapping(list, end, asize, CH_MEM_KDATA, &addr, True)) == Null || !addr) {
        EfiFreePool(map);
        return EFI_OUT_OF_RESOURCES;
    }

    mmap = (CHBootInfoMemMap*)addr;
    end += asize;

    /* Again, we probably overallocated, so let's clean the memory just to make sure (and this time we're almost sure
     * that we overallocated lol). */

    EfiZeroMemory(mmap, asize);

    for (UIntN i = 0; i < mcount; i++) {
        EfiMemoryDescriptor *desc = (EfiMemoryDescriptor*)((UIntN)map + i * dsize);
        UIntN base = (UIntN)desc->PhysicalStart, size = (UIntN)desc->NumberOfPages << 12;

        /* Make sure to save the highest (and lowest) physical address + the actual memory size. */

        if (base < minaddr) {
            minaddr = base;
        }

        if (base + size > maxaddr) {
            maxaddr = base + size;
        }

        switch (desc->Type) {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiConventionalMemory: {
            maxsize += size;
            break;
        }
        }
    }

    /* We can free the memory map that we read, as it is (probably) still going to change. */

    EfiFreePool(map);

    /* And now that we know where the physical memory starts and end, we can allocate/map/reserve/add a region that
     * the kernel expects, where it will put a list of physical memory regions. */
    
    UIntN regv = (UIntN)end;

    asize = (maxaddr - minaddr) >> 12;
#ifdef _WIN64
    asize += ((maxaddr - minaddr) >> 22) * 0x90;
#else
    asize += ((maxaddr - minaddr) >> 22) * 0x88;
#endif
    asize = (asize + 0xFFF) & ~0xFFF;

    if ((list = CHAddMapping(list, end, asize, CH_MEM_KDATA, &addr, True)) == Null || !addr) {
        EfiFreePool(map);
        return EFI_OUT_OF_RESOURCES;
    }

    end += asize;

    /* Fill in what we already can of the boot info struct. */

    bi->Magic = CH_BOOT_INFO_MAGIC;
    bi->KernelStart = start;
    bi->RegionsStart = regv;
    bi->EfiTempAddress = (UIntN)ArchJumpIntoCHicago;
    bi->MinPhysicalAddress = minaddr;
    bi->MaxPhysicalAddress = maxaddr;
    bi->PhysicalMemorySize = maxsize;
    bi->MemoryMap.Entries = mmapv;
    bi->BootImage.Size = size;
    bi->BootImage.Index = Entry->CHicago.ImageIndex;
    bi->BootImage.Data = nbufv;
    bi->FrameBuffer.Width = EfiGop->Mode->Info->Width;
    bi->FrameBuffer.Height = EfiGop->Mode->Info->Height;
    bi->FrameBuffer.Size = EfiGop->Mode->FrameBufferSize;
    bi->FrameBuffer.Address = fbv;

    /* Now we can create the page directory, and finish filling the boot info struct. */

    Void *dir;

    if (EFI_ERROR((status = ArchInitCHicagoMmu(feat, &list, &dir)))) {
        return status;
    }

    bi->KernelEnd = end;
    bi->Directory = dir;

    /* Last things remaining is the memory map, now that we are finished, the memory map is in its final state, and we
     * can fill our copy and save it into the boot info struct. But first, we have to get it again. */

    if ((map = EfiGetMemoryMap(&mcount, &mkey, &dsize, &dver)) == Null) {
        EfiDrawString("Couldn't get the memory map (the system may be out of memory).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        return EFI_OUT_OF_RESOURCES;
    }

    for (UIntN i = 0; i < mcount; i++) {
        EfiMemoryDescriptor *desc = (EfiMemoryDescriptor*)((UIntN)map + i * dsize);
        UIntN base = (UIntN)desc->PhysicalStart, size = (UIntN)desc->NumberOfPages << 12;
        UInt8 type = 0xFF;

        switch (desc->Type) {
        case EfiReservedMemoryType:
        case EfiUnusableMemory:
        case EfiACPIReclaimMemory:
        case EfiACPIMemoryNVS:
        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
        case EfiPalCode:
        case EfiPersistantMemory: {
            type = CH_MEM_RES;
            break;
        }
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
        case EfiConventionalMemory: {
            type = CH_MEM_FREE;
            break;
        }
        }

        /* If type is still on its default value, this is an entry that we can't handle, or is a LoaderCode/Data
         * entry, that may contain kernel code/data, and we have to handle those in a different way. */

        if (type != 0xFF) {
            mmap[mmapc].Base = base;
            mmap[mmapc].Count = size >> 12;
            mmap[mmapc++].Type = type;
            continue;
        } else if (desc->Type != EfiLoaderCode && desc->Type != EfiLoaderData) {
            continue;
        }

        /* For LoaderCode/LoaderData, we need to see if there is some kernel region that matches the entry. */

s:      ;CHMapping *ent = CHGetMapping(list, base, base + size);

        if (ent == Null) {
            /* Doesn't match, just add it all as avaliable. */

            mmap[mmapc].Base = base;
            mmap[mmapc].Count = size >> 12;
            mmap[mmapc++].Type = CH_MEM_FREE;

            continue;
        } else if (ent->Physical == base) {
            /* The region base perfectly matches with the base of the mmap entry! */

            mmap[mmapc].Base = base;
            mmap[mmapc].Count = size >> 12;
            mmap[mmapc++].Type = ent->Type;

            continue;
        } else if (ent->Physical < base && (mmapc == 0 || mmap[mmapc - 1].Type != ent->Type)) {
            /* Now, this be actually broken, it seems to work, but it may actually be broken (maybe I should later
             * fizzle this around to test if there isn't anything broken?). */

            EfiDrawString("The memory map seems to be corrupted.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
            EfiFreePool(map);

            return status;
        } else if (ent->Physical < base) {
            /* The region starts before the mmap entry. */

            UIntN add = (size <= ent->Size - (base - ent->Physical)) ? size : ent->Size - (base - ent->Physical);
            
            mmap[mmapc - 1].Count += add >> 12;
            size -= add;
            base += add;

            if (size == 0) {
                continue;
            }

            goto s;
        }

        /* The region starts after, let's split it into two. */

        mmap[mmapc].Base = base;
        mmap[mmapc].Count = (ent->Physical - base) >> 12;
        mmap[mmapc].Type = CH_MEM_FREE;
        size -= ent->Physical - base;
        base = ent->Physical;

        goto s;
    }

    bi->MemoryMap.Count = mmapc;

    EfiFreePool(map);

    /* Now we can exit the EFI environment (and remember that after that no more boot services, not runtime services,
     * as we're not going to remap the RS memory). */

    EfiFillScreen(0x00, 0x00, 0x00);
    EfiBS->GetMemoryMap(&msize, Null, &mkey, &dsize, Null);

    while (EFI_ERROR((status = EfiBS->ExitBootServices(EfiIH, mkey)))) {
        EfiBS->GetMemoryMap(&msize, Null, &mkey, &dsize, Null);
    }

    /* Jump into the kernel, and if the function fails/returns (somehow), HANG. */

    ArchJumpIntoCHicago(bi, biv, entry, feat);
    
    while (True) ;
}
