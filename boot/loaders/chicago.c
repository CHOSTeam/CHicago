/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 16:41 BRT
 * Last edited on July 09 of 2021 at 10:24 BRT */

#include <arch.h>
#include <efi/lib.h>
#include <efi/rng.h>
#include <elf.h>

SiaFile *CHGetKernel(SiaHeader *Header, UIntN Size, UInt16 Type, UInt16 *Features) {
    /* Default/generic GetBestFit for when we just have to find the first with the type matching (without worrying about
     * fallback nor anything like that). */

    if (Header == Null || Features == Null) return Null;

    for (UIntN i = 0; i < sizeof(Header->KernelImages) / sizeof(UInt64); i++) {
        if (Header->KernelImages[i] < sizeof(SiaHeader) || Header->KernelImages[i] + sizeof(SiaFile) >= Size) continue;
        SiaFile *file = (SiaFile*)((UIntN)Header + Header->KernelImages[i]);
        if (!file->Offset || !file->Size || file->Offset + file->Size > Size || !(file->Flags & Type)) continue;
        *Features = Type;
        return file;
    }

    return Null;
}

EfiStatus CHWalkMmuLevel(UIntN *Level, Mapping **List, EfiVirtualAddress Virtual, UInt8 Shift, UInt16 Mask,
                         UInt16 Flags, Boolean (*IsPresent)(UIntN), Boolean (*IsHuge)(UIntN), UIntN *Out) {
    /* This is just the arm64/amd64 MmuWalkLevel function, but made a bit more arch-independent (at least we can use it
     * on both arm64 and amd64). */

    UIntN tbl = Level[(Virtual >> Shift) & Mask];

    if (!IsPresent(tbl)) {
        /* Allocate/reserve memory for the table entry. */

        EfiPhysicalAddress addr;
        if ((*List = AddMapping(*List, UINTN_MAX, &addr, 0x1000, 0)) == Null || !addr) return EFI_OUT_OF_RESOURCES;
        EfiZeroMemory((Void*)addr, 0x1000);
        Level[(Virtual >> Shift) & Mask] = addr | Flags;
        *Out = addr;
    } else if (IsHuge(tbl)) {
        EfiDrawString("The MMU paging structures got corrupted during the initialization process.",
                      5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    } else *Out = tbl & ~0xFFF;

    return EFI_SUCCESS;
}

EfiStatus CHMapKernel(Void *Directory, Mapping **List, EfiStatus (*Map)(Void*, Mapping**, Mapping*)) {
    /* This function just maps the kernel and the jump address. */

    Mapping sent = { 0x1000, MAP_KERNEL, MAP_VIRT | MAP_EXEC, (UInt64)ArchJumpIntoCHicago & ~0xFFF,
                     (UInt64)ArchJumpIntoCHicago & ~0xFFF, Null, Null };
    EfiStatus status = Map(Directory, List, &sent);

    if (EFI_ERROR(status)) return status;

    for (Mapping *ent = *List; ent != Null; ent = ent->Next)
        if (EFI_ERROR((status = Map(Directory, List, ent)))) return status;

    return EFI_SUCCESS;
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
        if (*Last >= SourceSize || !*Last) return EFI_BUFFER_TOO_SMALL;
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

	if (EFI_ERROR(status)) return status;

	/* Now we can just follow all the links in the file struct, while copying all the data. */

	while (Length) {
	    if (last >= SourceSize || !last) return EFI_BUFFER_TOO_SMALL;

	    data = (SiaData*)&SourceAddress[last];
	    UInt64 size = sizeof(data->Data) - skip;

	    if (size > Length) size = Length;

	    EfiCopyMemory(&OutAddress[cur], &data->Data[skip], size);

	    skip = 0;
	    cur += size;
	    Length -= size;
	    last = data->Next;
	}

	return EFI_SUCCESS;
}

static Boolean IsDecimal(Char8 Value) {
    return Value >= '0' && Value <= '9';
}

static Boolean IsHex(Char8 Value) {
    return IsDecimal(Value) || (Value >= 'a' && Value <= 'f') || (Value >= 'A' && Value <= 'F');
}

static UIntN FromHex(Char8 Value) {
    return IsDecimal(Value) ? Value - '0' : ((Value >= 'a' && Value <= 'f' ? (Value - 'a') : (Value - 'A')) + 10);
}

static Void SiaLoadSymbols(UInt8 *Buffer, UIntN Size, SiaFile *File, UIntN *End, UIntN *Count, Mapping **List) {
    /* First, read the symbol list (it is a big string, so we gonna need to parse it). */

    if (!File->Size) return;

    UIntN i = 0, start = 0, cur = 0, namesize = 0;
    Char8 *syms = EfiAllocateZeroPool(File->Size + 1);

    if (syms == Null || EFI_ERROR(SiaReadFile(Buffer, Size, File, 0, File->Size, (UInt8*)syms))) {
        EfiFreePool(syms);
        return;
    }

    /* Now, let's do two important things at the same time: First, check if all the entries are valid, second, store
     * the length of all names combined (we gonna have a single big names section after the symbols structs). Of course,
     * before starting that, we first have to parse the first line, which SHOULD have the amount of lines/entries. */

    while (cur < File->Size && IsDecimal(syms[cur])) *Count = (*Count * 10) + syms[cur++] - '0';

    if (!cur || !*Count || syms[cur++] != '\n') {
        EfiFreePool(syms);
        *Count = 0;
        return;
    }

    for (start = cur; i < *Count && cur < File->Size && syms[cur]; namesize++, cur++, i++) {
        /* The entries are all on the format of '<address in hex> <size in hex> <ch> <name>', the hex values don't
         * have the 0x on the start, and there is only one entry per line (everything after the ch is parsed as the
         * name, and not validated). */

        if (!IsHex(syms[cur])) {
            EfiFreePool(syms);
            *Count = 0;
            return;
        }

        for (; cur < File->Size && IsHex(syms[cur]); cur++) ;

        if (cur + 2 >= File->Size || syms[cur++] != ' ' || !IsHex(syms[cur])) {
            EfiFreePool(syms);
            *Count = 0;
            return;
        }

        for (; cur < File->Size && IsHex(syms[cur]); cur++) ;

        if (cur + 2 >= File->Size || syms[cur++] != ' ' || !syms[cur] || syms[cur] == '\n') {
            EfiFreePool(syms);
            *Count = 0;
            return;
        }

        for (; cur < File->Size && syms[cur] && syms[cur] != '\n'; cur++, namesize++) ;

        if (i + 1 < *Count && (cur >= File->Size || syms[cur] != '\n')) {
            EfiFreePool(syms);
            *Count = 0;
            return;
        }
    }

    if (i < *Count) return;

    /* Now we just need to alloc the symbols region, and fill it (parse the addresses, write the names to the right
     * place, etc). */

    EfiPhysicalAddress addr;
    UIntN size = (*Count * sizeof(CHBootInfoSymbol) + namesize + 0xFFF) & ~0xFFF;

    if ((*List = AddMapping(*List, *End, &addr, size, MAP_VIRT)) == Null || !addr) {
        EfiFreePool(syms);
        *Count = 0;
        return;
    }

    EfiZeroMemory((Void*)addr, size);

    /* Also, as now everything is allocated, and we know the symbol entries are valid, we can't fail anymore lol. */

    CHBootInfoSymbol *ksyms = (CHBootInfoSymbol*)addr;
    Char8 *namesp = (Char8*)(addr + *Count * sizeof(CHBootInfoSymbol)),
          *namesv = (Char8*)(*End + *Count * sizeof(CHBootInfoSymbol));

    for (i = 0, cur = start; i < *Count && cur < File->Size; *namesp++ = 0, namesv++, cur++, i++) {
        CHBootInfoSymbol *sym = &ksyms[i];

        sym->Start = sym->End = 0;
        sym->Name = namesv;

        while (IsHex(syms[cur])) sym->Start = (sym->Start * 16) + FromHex(syms[cur++]);

        cur++;

        while (IsHex(syms[cur])) sym->End = (sym->End * 16) + FromHex(syms[cur++]);

        cur += 3;
        sym->End += sym->Start;
        while (cur < File->Size && syms[cur] && syms[cur] != '\n') *namesp++ = syms[cur++], namesv++;
    }

    *End += size;

    EfiFreePool(syms);
}

static EfiStatus SiaLoadKernel(UInt8 *Buffer, UIntN Size, UInt16 *Features, UIntN *Entry, UIntN *Start, UIntN *End,
                               UIntN *SymStart, UIntN *SymCount, Mapping **List) {
    /* First, check if the SIA file is valid and not corrupted. */

    EfiStatus status = SiaCheck(Buffer, Size);

    if (EFI_ERROR(status)) return status;

    /* Now get the optimal kernel image (we can have multiple kernel images, so we need to do this). */

    SiaHeader *hdr = (SiaHeader*)Buffer;
    SiaFile *file = ArchGetBestFitCHicago(hdr, Size, Features);

    if (file == Null) {
        EfiDrawString("Couldn't find a compatible kernel image.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    }

    /* Now we can read the ELF header, and make sure the file is actually a valid kernel for this architecture. */

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

    ElfProgHeader phdrs[ehdr.ProgHeaderCount];

    if (EFI_ERROR((status = SiaReadFile(Buffer, Size, file, ehdr.ProgHeaderOffset,
                                        ehdr.ProgHeaderCount * sizeof(ElfProgHeader), (UInt8*)phdrs)))) {
        EfiDrawString("Couldn't read the kernel ELF program headers.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return status;
    }

    /* The sections all have both the virtual address (as we're supposed to jump into the kernel after enabling
     * paging) and the physical address, we need to load everything into the physical address space. */

    for (UIntN c = 0, i = 0; i < ehdr.ProgHeaderCount; i++) {
        /* And here we are fixing 0x1000 as the page size again lol. */

        EfiPhysicalAddress addr;
        ElfProgHeader *phdr = &phdrs[i];
        UIntN size = (phdr->MemSize + 0xFFF) & ~0xFFF;

        /* The size in memory and size in the file may be different, so let's clean it first, and load what we need
         * into memory after that. */

        if ((*List = AddMapping(*List, phdr->VirtAddress, &addr, size, MAP_VIRT |
                                ((phdr->Flags & 0x01) ? MAP_EXEC : 0) |
                                ((phdr->Flags & 0x02) ? MAP_WRITE : 0))) == Null || !addr) return status;
        else if (phdr->VirtAddress < *Start) *Start = phdr->VirtAddress;
        if (phdr->VirtAddress + size > *End) *End = phdr->VirtAddress + size;

        EfiZeroMemory((Void*)addr, size);

        if (phdr->FileSize && EFI_ERROR((status = SiaReadFile(Buffer, Size, file, phdr->Offset, phdr->FileSize,
                                                              (UInt8*)addr)))) {
            EfiDrawString("Couldn't read one of the kernel sections.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
            return status;
        }

        /* If this is the first read-only section of the kernel, we have to setup the stack guard variable at the
         * start. */

        if (!c && !(phdr->Flags & 0x03)) {
            UIntN val;
            EfiRng *rng;
            EfiTime time;

            if (EFI_ERROR(EfiBS->LocateProtocol(&EfiRngGuid, Null, (Void**)&rng)) ||
                EFI_ERROR(rng->GetRandom(rng, Null, sizeof(UIntN), (UInt8*)&val))) {
#ifdef _WIN64
                val = 0x595E9FBD94FDA766;
#else
                val = 0xE2DEE396;
#endif

                if (!EFI_ERROR(EfiRS->GetTime(&time, Null))) {
                    val *= ((time.Year * 0x1DA9C00) + (time.Month * 0x278D00) + (time.Day * 0x15180) +
                            (time.Hour * 0xE10) + (time.Minute * 0x3C) + time.Second + time.Nanosecond) * 0x1337;
                }
            }

            *((UIntN*)addr) = val;
            c++;
        }
    }

    *Entry = ehdr.Entry;

    /* Now we need to init the symbol table, for this let's call another function (which does only that), it doesn't
     * return any status code (as it's okay if we don't load the symtab/fail to load it). */

    if (file->Next) {
        *SymStart = *End;
        SiaLoadSymbols(Buffer, Size, (SiaFile*)&Buffer[file->Next], End, SymCount, List);
    }

    return EFI_SUCCESS;
}

EfiStatus LdrStartCHicago(MenuEntry *Entry) {
    /* Entry should be valid, but let's make sure. */

    if (Entry == Null || Entry->Path == Null || Entry->Type != MenuEntryCHicago) return EFI_INVALID_PARAMETER;

    /* Before anything else, read in the EFI memory map to initialize our mapping list. */

    Mapping *list = InitMappings();
    if (list == Null) return EFI_OUT_OF_RESOURCES;

    /* Let's read in the SIA file (in whatever path it is), validate it, and load the best fit kernel into memory
     * (everything but loading in the file into memory is done by SiaLoadKernel). */

    UInt8 *buf;
    UInt16 feat;
    EfiFile *file;
    UIntN size, entry, start = UINTN_MAX, end = 0, symstart, symcnt = 0;

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

    if (EFI_ERROR((status = SiaLoadKernel(buf, size, &feat, &entry, &start, &end, &symstart, &symcnt, &list)))) {
        EfiFreePool(buf);
        return status;
    }

    /* CHicago expects the start function to be called while a special boot struct is passed as the first arg, let's
     * reserve the space for said boot struct. */

    UInt64 asize = (sizeof(CHBootInfo) + 0xFFF) & ~0xFFF;
    EfiPhysicalAddress addr;
    UIntN biv = (UIntN)end;
    CHBootInfo *bi;

    if ((list = AddMapping(list, end, &addr, asize, MAP_VIRT | MAP_WRITE)) == Null || !addr) {
        EfiFreePool(buf);
        return EFI_OUT_OF_RESOURCES;
    }

    bi = (CHBootInfo*)addr;
    end += asize;

    EfiZeroMemory(bi, asize);

    /* Realloc the boot SIA image file contents (to a place that the kernel will be able to access post
     * ExitBootServices). */

    UInt8 *nbuf, *nbufv = (UInt8*)end;

    if ((list = AddMapping(list, end, &addr, (asize = (size + 0xFFF) & ~0xFFF), MAP_VIRT)) == Null || !addr) {
        EfiFreePool(buf);
        return EFI_OUT_OF_RESOURCES;
    }

    nbuf = (UInt8*)addr;
    end += asize;

    /* Zero the memory as we probably over allocated (pretty hard to not happen). */

    EfiZeroMemory(nbuf, asize);
    EfiCopyMemory(nbuf, buf, size);
    EfiFreePool(buf);

    /* Map the framebuffer (back buffer) into virtual memory, and alloc memory for the front buffer. */

    addr = EfiGop->Mode->FrameBufferBase;

    UIntN backv = (UIntN)end, frontv = (UIntN)end + (asize = (EfiGop->Mode->FrameBufferSize + 0xFFF) & ~0xFFF);

    if ((list = AddMapping(list, end, &addr, asize, MAP_VIRT | MAP_WRITE | MAP_DEVICE)) == Null || !addr)
        return EFI_OUT_OF_RESOURCES;
    else if ((list = AddMapping(list, end + asize, &addr, asize, MAP_VIRT | MAP_WRITE)) == Null || !addr)
        return EFI_OUT_OF_RESOURCES;

    end += asize * 2;

    /* Get and the ACPI tables location (EfiGetAcpiTables return the start of the RSDT/XSDT). */

    Boolean xsdt;
    UIntN sdt = (EfiPhysicalAddress)EfiGetAcpiTables(&xsdt);

    if (!sdt) {
        EfiDrawString("Couldn't get the ACPI RDST/XSDT.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    }

    /* We need to grab the max addressable physical address (+ first addressable address), and also try to grab the
     * memory size (though it's not really accurate in this way). */

    UInt32 dver = 0;
    IntN mmapc = -1;
    UIntN dsize = 0, msize = 0, mcount = 0, mkey = 0;
    UInt64 minaddr = UINT64_MAX, maxaddr = 0, maxsize = 0;
    EfiMemoryDescriptor *map = EfiGetMemoryMap(&mcount, &mkey, &dsize, &dver);

    if (map == Null) {
        EfiDrawString("Couldn't get the memory map (the system may be out of memory).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        return EFI_OUT_OF_RESOURCES;
    }

    for (UIntN i = 0; i < mcount; i++) {
        EfiMemoryDescriptor *desc = (EfiMemoryDescriptor*)((UIntN)map + i * dsize);
        UInt64 sz = desc->NumberOfPages << 12;

        if (desc->PhysicalStart < minaddr) minaddr = desc->PhysicalStart;
        if (desc->PhysicalStart + sz > maxaddr) maxaddr = desc->PhysicalStart + sz;

        switch (desc->Type) {
        case EfiBootServicesData: case EfiBootServicesCode: case EfiRuntimeServicesData: case EfiRuntimeServicesCode:
        case EfiLoaderData: case EfiLoaderCode: case EfiConventionalMemory: maxsize += sz; break;
        }
    }

    /* We can free the memory map that we read, as it is (probably) still going to change. */

    EfiFreePool(map);

    /* And now that we know where the physical memory starts and end, we can allocate/map/reserve/add a region that
     * the kernel expects, where it will put a list of physical memory regions. */

    UIntN regv = (UIntN)end;

    if ((list = AddMapping(list, end, &addr,
                           asize = (((maxaddr - minaddr) >> 12) * (sizeof(UIntN) * 4 + 1) + 0xFFF) & ~0xFFF,
                           MAP_VIRT | MAP_WRITE)) == Null || !addr) {
        EfiFreePool(map);
        return EFI_OUT_OF_RESOURCES;
    }

    end += asize;

    /* Fill in what we already can of the boot info struct. */

    bi->Magic = CH_BOOT_INFO_MAGIC;
    bi->KernelStart = start;
    bi->PhysMgrStart = regv;
    bi->EfiTempAddress = (UIntN)ArchJumpIntoCHicago;
    bi->MinPhysicalAddress = minaddr;
    bi->MaxPhysicalAddress = maxaddr;
    bi->PhysicalMemorySize = maxsize;
    bi->Acpi.Extended = xsdt;
    bi->Acpi.Size = ((UInt32*)sdt)[1];
    bi->Acpi.Sdt = sdt;
    bi->Symbols.Count = symcnt;
    bi->Symbols.Start = symcnt ? (CHBootInfoSymbol*)symstart : Null;
    bi->BootImage.Size = size;
    bi->BootImage.Index = Entry->CHicago.ImageIndex;
    bi->BootImage.Data = nbufv;
    bi->FrameBuffer.Width = EfiGop->Mode->Info->Width;
    bi->FrameBuffer.Height = EfiGop->Mode->Info->Height;
    bi->FrameBuffer.BackBuffer = backv;
    bi->FrameBuffer.FrontBuffer = frontv;

    /* Now we can create the page directory, and finish filling the boot info struct. */

    Void *dir;

    if (EFI_ERROR((status = ArchInitCHicagoMmu(feat, &list, &dir)))) return status;

    bi->KernelEnd = end;
    bi->Directory = dir;

    /* Last things remaining is the memory map, now that we are finished, the memory map is in its final state, and we
     * can fill our copy and save it into the boot info struct. But first, we have to get it again. */

    for (Mapping *cur = list; cur != Null; cur = cur->Next) {
        UInt8 type = cur->Type == MAP_RES ? CH_MEM_RES : (cur->Type == MAP_FREE ? CH_MEM_FREE :
                     (cur->Flags & MAP_DEVICE ? CH_MEM_DEV : (cur->Flags & MAP_EXEC ? CH_MEM_KCODE :
                     (cur->Flags & MAP_WRITE ? CH_MEM_KDATA : CH_MEM_KDATA_RO))));

        if (mmapc <= 0 ||
            cur->Physical != bi->MemoryMap.Entries[mmapc].Base + (bi->MemoryMap.Entries[mmapc].Count << 12) ||
            type != bi->MemoryMap.Entries[mmapc].Type) {
            bi->MemoryMap.Entries[++mmapc].Base = cur->Physical;
            bi->MemoryMap.Entries[mmapc].Count = cur->Size >> 12;
            bi->MemoryMap.Entries[mmapc].Type = type;
        } else bi->MemoryMap.Entries[mmapc].Count += cur->Size >> 12;
    }

    bi->MemoryMap.Count = mmapc;

    /* Now we can exit the EFI environment (and remember that after that no more boot services, not runtime services,
     * as we're not going to remap the RS memory). */

    EfiFillScreen(0x00, 0x00, 0x00);
    EfiBS->GetMemoryMap(&msize, Null, &mkey, &dsize, Null);

    while (EFI_ERROR((status = EfiBS->ExitBootServices(EfiIH, mkey))))
        EfiBS->GetMemoryMap(&msize, Null, &mkey, &dsize, Null);

    /* Jump into the kernel, and if the function fails/returns (somehow), HANG. */

    ArchJumpIntoCHicago(bi, biv, entry, feat);
    
    while (True) ;
}
