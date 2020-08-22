/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 21 of 2020, at 00:03 BRT
 * Last edited on July 19 of 2020, at 13:48 BRT */

#include <elf.h>
#include <sia.h>

static EFI_STATUS SiaCheck(CHAR8 *Buffer, UINTN Size) {
	/* SIA (System Image Archive) is a TAR-like file format, used here on the osldr as the
	 * kernel/initrd container. We need to check the header to make sure it is valid, the process is:
	 * Check if the Size is, at least, the size of the SIA_HEADER struct.
	 * Check if the header magic number is the correct one.
	 * Check if the free file/data offset is valid.
	 * Check if the root directory offset is valid (can't' be zero).
	 * And, for us, we also need to check if the kernel file entry offset is valid. */
	
	if (Size < sizeof(SIA_HEADER)) {
		Print(L"EFI Loader Error: The SIA file is invalid.\n");
		return EFI_BUFFER_TOO_SMALL;
	}
	
	SIA_HEADER *Header = (SIA_HEADER*)Buffer;
	
	if (Header->Magic != SIA_MAGIC) {
		Print(L"EFI Loader Error: The SIA file is invalid.\n");
		return EFI_INVALID_PARAMETER;
	} else if (Header->FreeFileCount != 0 && (Header->FreeFileOffset < sizeof(SIA_HEADER) ||
			   Header->FreeFileOffset + Header->FreeFileCount * sizeof(SIA_FILE) >= Size)) {
		Print(L"EFI Loader Error: The SIA file is invalid.\n");
		return EFI_INVALID_PARAMETER;
	} else if (Header->FreeDataCount != 0 && (Header->FreeDataOffset < sizeof(SIA_HEADER) ||
			   Header->FreeDataOffset + Header->FreeDataCount * sizeof(SIA_DATA) >= Size)) {
		Print(L"EFI Loader Error: The SIA file is invalid.\n");
		return EFI_INVALID_PARAMETER;
	} else if (Header->RootOffset < sizeof(SIA_HEADER) || Header->RootOffset + sizeof(SIA_FILE) >= Size) {
		Print(L"EFI Loader Error: The SIA file is invalid.\n");
		return EFI_INVALID_PARAMETER;
	} else if (!(Header->Info & SIA_INFO_KERNEL) ||
			   Header->KernelOffset < sizeof(SIA_HEADER) || Header->KernelOffset + sizeof(SIA_FILE) >= Size) {
		Print(L"EFI Loader Error: The SIA file doesn't contain a kernel image.\n");
		return EFI_INVALID_PARAMETER;
	}
	
#ifdef DBG
	Print(L"SIA File Information\n");
	Print(L"    Magic Number: 0x%08x\n", Header->Magic);
	Print(L"    UUID: %x%x%x%x-%x%x-%x%x-%x%x-%x%x%x%x%x%x\n",
		  Header->UUID[0], Header->UUID[1], Header->UUID[2], Header->UUID[3], Header->UUID[4], Header->UUID[5],
		  Header->UUID[6], Header->UUID[7], Header->UUID[8], Header->UUID[9], Header->UUID[10], Header->UUID[11],
		  Header->UUID[12], Header->UUID[13], Header->UUID[14], Header->UUID[15]);
#endif
	
	return EFI_SUCCESS;
}

static EFI_STATUS SiaGoToOffset(CHAR8 *SourceAddress, UINTN SourceSize, SIA_FILE *File, UINT64 Offset, UINT64 *Last) {
	/* By default, init the output argument (Last) with the offset of the start of the file. */
	
	SIA_DATA *Data;
	
	*Last = File->Offset;
	
	/* Now, let's follow all the links until we find the SIA_DATA struct that we want. */
	
	while (Offset >= sizeof(Data->Contents)) {
		if (*Last >= SourceSize || *Last == 0) {
			return EFI_BUFFER_TOO_SMALL;
		}
		
		Data = (SIA_DATA*)&SourceAddress[*Last];
		Offset -= sizeof(Data->Contents);
		*Last = Data->Next;
	}
	
	return EFI_SUCCESS;
}

static EFI_STATUS SiaReadFile(CHAR8 *SourceAddress, UINTN SourceSize, SIA_FILE *File, UINT64 Offset, UINT64 Length, 
							  CHAR8 *OutAddress) {
	SIA_DATA *Data;
	UINT64 Last = 0;
	UINT64 Current = 0;
	UINT64 Skip = Offset % sizeof(Data->Contents);
	
	/* First, we need to get the location where this file starts, it could be as simple as just using File->Offset,
	 * but we need to account for the Offset argument. */
	
	EFI_STATUS Status = SiaGoToOffset(SourceAddress, SourceSize, File, Offset, &Last);
	
	if (EFI_ERROR(Status)) {
		return Status;
	}
	
	/* Now, we can just follow all the links in the file struct, while copying all the data. */
	
	while (Length) {
		if (Last >= SourceSize || Last == 0) {
			return EFI_BUFFER_TOO_SMALL;
		}
		
		Data = (SIA_DATA*)&SourceAddress[Last];
		
		UINT64 Size = sizeof(Data->Contents) - Skip;
		
		if (Size > Length) {
			Size = Length;
		}
		
		CopyMem(&OutAddress[Current], &Data->Contents[Skip], Size);
		
		Skip = 0;
		Current += Size;
		Length -= Size;
		Last = Data->Next;
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS SiaLoadKernel(CHAR8 *Buffer, UINTN Size, UINTN *Entry, UINTN *StartPhys, UINTN *StartVirt,
						 UINTN *EndPhys, UINTN *EndVirt) {
	/* We need to check if this is really a SIA file, if it's a valid SIA file and if it really
	 * contains the kernel image. */
	
	EFI_STATUS Status = SiaCheck(Buffer, Size);
	
	if (EFI_ERROR(Status)) {
		return Status;
	}
	
	/* Getting the SIA_FILE struct of the kernel file is pretty simple, and reading the file is just
	 * a matter of following all the links starting at ->Offset.
	 * Just to make sure, let's check the ->Flags of the file, it should have been created without the
	 * DIRECTORY flag, and as READ+EXEC. */
	
	SIA_HEADER *Header = (SIA_HEADER*)Buffer;
	
	SIA_FILE *File = (SIA_FILE*)&Buffer[Header->KernelOffset];
	
	if (File->Flags != (SIA_FLAGS_READ | SIA_FLAGS_EXEC)) {
		Print(L"EFI Loader Error: The kernel image should have the read and the exec flags set.\n");
		return EFI_INVALID_PARAMETER;
	}
	
	/* Check if the size isn't too crazy (compared to the .SIA file size), and if the ->Offset is also
	 * valid. */
	
	if (File->Offset == 0 || File->Size == 0 ||
		File->Offset + ((File->Size + sizeof(SIA_DATA) - 1) & -sizeof(SIA_DATA)) > Size) {
		Print(L"EFI Loader Error: The kernel image entry is invalid/corrupted.\n");
		return EFI_INVALID_PARAMETER;
	}
	
	/* Time to read the ELF header, and make sure that everything is OK. */
	
	ELF_HEADER ElfHeader;
	
	if (EFI_ERROR((Status = SiaReadFile(Buffer, Size, File, 0, sizeof(ELF_HEADER), (CHAR8*)&ElfHeader)))) {
		Print(L"EFI Loader Error: Couldn't read the kernel ELF header.\n");
		return Status;
	} else if (CompareMem(ElfHeader.Ident, "\177ELF", 4)) {
		Print(L"EFI Loader Error: The kernel file is corrupted/not an ELF file.\n");
		return EFI_INVALID_PARAMETER;
	} else if (ElfHeader.Type != 2) {
		Print(L"EFI Loader Error: The kernel file is not a valid kernel file.\n");
		return EFI_INVALID_PARAMETER;
	} else if (ElfHeader.Machine != ELF_MACHINE) {
		Print(L"EFI Loader Error: The kernel file is not made for this architecture.\n");
		return EFI_INVALID_PARAMETER;
	} else if (ElfHeader.Version == 0) {
		Print(L"EFI Loader Error: The kernel file is corrupted.\n");
		return EFI_INVALID_PARAMETER;
	}
	
	/* To load all the sections, we NEED the program headers, the size and amount isn't fixed, so we need
	 * to allocate at runtime (using AllocatePool). */
	
	ELF_PHDR *ProgHeaders = AllocatePool(ElfHeader.ProgHeaderCount * ElfHeader.ProgHeaderEntSize);
	
	if (ProgHeaders == NULL) {
		Print(L"EFI Loader Error: Couldn't allocate memory for reading the kernel file.\n");
		return EFI_OUT_OF_RESOURCES;
	} else if (EFI_ERROR((Status = SiaReadFile(Buffer, Size, File, ElfHeader.ProgHeaderOffset,
											   ElfHeader.ProgHeaderCount * ElfHeader.ProgHeaderEntSize,
											   (CHAR8*)ProgHeaders)))) {
		Print(L"EFI Loader Error: Couldn't read the kernel file.\n");
		return Status;
	}
	
	/* Now that we have the program headers, we can load everything in, the PhysAddress of the section says
	 * which is the right physical address to load everything in, so we just need to use the AllocatePages function
	 * (gBS->AllocatePages) and then load the section. */
	
	for (UINTN i = 0; i < ElfHeader.ProgHeaderCount; i++) {
		ELF_PHDR *ProgHeader = (ELF_PHDR*)&ProgHeaders[i];
		EFI_PHYSICAL_ADDRESS Location = (EFI_PHYSICAL_ADDRESS)ProgHeader->PhysAddress;
		UINTN SectPages = (ProgHeader->MemSize + 0xFFF) / 0x1000, SectSize = SectPages * 0x1000;
		
		if (ProgHeader->PhysAddress + SectSize > *EndPhys) {
			*EndPhys = ProgHeader->PhysAddress + SectSize;
			*EndVirt = ProgHeader->VirtAddress + SectSize;
		}
		
		if (ProgHeader->PhysAddress < *StartPhys) {
			*StartPhys = ProgHeader->PhysAddress;
			*StartVirt = ProgHeader->VirtAddress;
		}
		
		if (EFI_ERROR((Status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, SectPages, &Location)))) {
			Print(L"EFI Loader Error: Couldn't allocate memory for reading the kernel file.\n");
			return Status;
		}
		
		ZeroMem((CHAR8*)Location, SectSize);
		
		if (EFI_ERROR((Status = SiaReadFile(Buffer, Size, File,
											ProgHeader->Offset, ProgHeader->FileSize, (CHAR8*)Location)))) {
			Print(L"EFI Loader Error: Couldn't read the kernel file.\n");
			return Status;
		}
	}
	
	/* Now we already have everything loaded in, free the memory we allocated for reading the program
	 * header, and, if compiled with the DBG flag, print some info. */
	
	FreePool(ProgHeaders);
	
	*Entry = ElfHeader.Entry;
	
#ifdef DBG
	Print(L"Loaded the kernel image\n");
	Print(L"    Entry Point: 0x%x\n", *Entry);
#ifdef ARCH_64
	Print(L"    Starts at the physical address 0x%llx (virtual address 0x%llx)\n",
		  *StartPhys, *StartVirt);
	Print(L"    Ends at the physical address 0x%llx (virtual address 0x%llx)\n",
		  *EndPhys, *EndVirt);
#else
	Print(L"    Starts at the physical address 0x%x (virtual address 0x%x)\n",
		  *StartPhys, *StartVirt);
	Print(L"    Ends at the physical address 0x%x (virtual address 0x%x)\n",
		  *EndPhys, *EndVirt);
#endif
#endif
	
	return EFI_SUCCESS;
}
