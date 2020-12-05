/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 23 of 2020, at 22:49 BRT
 * Last edited on December 04 of 2020, at 15:01 BRT */

#include <loader.h>

static EFI_STATUS MapPage(UINT64 *PML4, UINTN *KernelEndPhys, UINTN *KernelEndVirt,
						  UINTN Phys, UINTN Virt) {
	/* We're doing all the mapping using 2MiB pages, so we need to alloc the entries (if necessary) in
	 * the PML4 and in the PDP. */
	
	UINT64 PML4E = (Virt >> 39) & 0x1FF, PDPE = (Virt >> 30) & 0x1FF,
		   PDE = (Virt >> 21) & 0x1FF;
	
	if (!(PML4[PML4E] & 0x01)) {
		EFI_PHYSICAL_ADDRESS Location = *KernelEndPhys;
		EFI_STATUS Status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, 1, &Location);
		
		if (EFI_ERROR(Status)) {
			Print(L"EFI Loader Error: Couldn't allocate memory for filling the page directory.\n");
			return Status;
		}
		
		ZeroMem((CHAR8*)Location, 0x1000);
		
		PML4[PML4E] = ((UINTN)Location & ~0xFFF) | 0x03;
		*KernelEndPhys += 0x1000;
		*KernelEndVirt += 0x1000;
	}
	
	UINT64 *PDP = (UINT64*)(PML4[PML4E] & ~0xFFF);
	
	if (!(PDP[PDPE] & 0x01)) {
		EFI_PHYSICAL_ADDRESS Location = *KernelEndPhys;
		EFI_STATUS Status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, 1, &Location);
		
		if (EFI_ERROR(Status)) {
			Print(L"EFI Loader Error: Couldn't allocate memory for filling the page directory.\n");
			return Status;
		}
		
		ZeroMem((CHAR8*)Location, 0x1000);
		
		PDP[PDPE] = ((UINTN)Location & ~0xFFF) | 0x03;
		*KernelEndPhys += 0x1000;
		*KernelEndVirt += 0x1000;
	}
	
	/* Just fill the right PD entry, setting the huge page bit, so the processor knows that it is not
	 * a pointer into some PT. */
	
	((UINT64*)(PDP[PDPE] & ~0xFFF))[PDE] = (Phys & ~0x1FFFFF) | 0x83;
	
	return EFI_SUCCESS;
}

EFI_STATUS CreateDirectory(UINTN *DirectoryPhys,
						   UINTN *KernelEndPhys, UINTN *KernelEndVirt,
						   UINTN FrameBufferPhys, UINTN *FrameBufferVirt, UINTN FrameBufferSize,
						   UINT64 MaxPhysicalAddress, UINTN *RegionsStartPhys, UINTN *RegionsStartVirt) {
	/* First, allocate space for the page directory. We need the physical address, we can (for now)
	 * easily discover what the virtual address is going to be. */
	
	EFI_PHYSICAL_ADDRESS Location = *KernelEndPhys;
	EFI_STATUS Status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, 1, &Location);
	
	if (EFI_ERROR(Status)) {
		Print(L"EFI Loader Error: Couldn't allocate memory for creating the page directory.\n");
		return Status;
	}
	
	*DirectoryPhys = *KernelEndPhys;
	*KernelEndPhys += 0x1000;
	*KernelEndVirt += 0x1000;
	
#ifdef DBG
	Print(L"Allocated the page directory at the physical address 0x%016llx\n", *DirectoryPhys);
#endif
	
	/* Now we can map the kernel (and the other structs that we built for the kernel), different from x86-32,
	 * here we use the MapPage function (which makes sure that everything is allocated correctly, and calculate
	 * the indexes for us). */
	
	UINT64 *Directory = (UINT64*)*DirectoryPhys;
	UINTN EfiMainAddress = (UINTN)EfiMain;
	UINTN Current = 0, FbCurrent = 0;
	
	if (EFI_ERROR((Status = MapPage(Directory, KernelEndPhys, KernelEndVirt, EfiMainAddress, EfiMainAddress)))) {
		return Status;
	}
	
	for (; Current < *KernelEndPhys; Current += 0x200000) {
		if (EFI_ERROR((Status = MapPage(Directory, KernelEndPhys, KernelEndVirt,
										Current, 0xFFFFFFFF80000000 + Current)))) {
			return Status;
		}
	}
	
	/* Now, map the framebuffer. */
	
	*KernelEndVirt = 0xFFFFFFFF80000000 + Current;
	*FrameBufferVirt = 0xFFFFFFFF80000000 + Current;
	
	for (; FbCurrent < FrameBufferSize; FbCurrent += 0x200000) {
		if (EFI_ERROR((Status = MapPage(Directory, KernelEndPhys, KernelEndVirt,
										FrameBufferPhys + FbCurrent, 0xFFFFFFFF80000000 + Current + FbCurrent)))) {
			return Status;
		}
	}
	
	/* Finally, map some extra pages, as the kernel expects the space that the PMM will be located (which is just
	 * after the memory map) to be already mapped. */
	
	UINTN RegionCount = MaxPhysicalAddress / 0x400000;
	
	*RegionsStartPhys = *KernelEndPhys;
	*RegionsStartVirt = 0xFFFFFFFF80000000 + Current + FbCurrent;
	
	for (; Current < *KernelEndPhys + (RegionCount * 0x104) +
									  (MaxPhysicalAddress / 0x1000) + 0x200000; Current += 0x200000) {
		if (EFI_ERROR((Status = MapPage(Directory, KernelEndPhys, KernelEndVirt,
										Current, 0xFFFFFFFF80000000 + Current + FbCurrent)))) {
			return Status;
		}
	}
	
	*KernelEndPhys = Current;
	*KernelEndVirt = 0xFFFFFFFF80000000 + Current + FbCurrent;
	
	/* Finally, create the recursive entry (so we can access/modify the page tables even with paging enabled). */
	
	Directory[510] = (*DirectoryPhys & ~0xFFF) | 0x03;
	
	return EFI_SUCCESS;
}
