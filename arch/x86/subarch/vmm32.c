/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 23 of 2020, at 22:49 BRT
 * Last edited on July 17 of 2020, at 23:22 BRT */

#include <loader.h>

EFI_STATUS CreateDirectory(UINTN *DirectoryPhys,
						   UINTN *KernelEndPhys, UINTN *KernelEndVirt,
						   UINTN FrameBufferPhys, UINTN *FrameBufferVirt, UINTN FrameBufferSize,
						   UINTN MaxPhysicalAddress, UINTN *RegionsStartPhys, UINTN *RegionsStartVirt) {
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
	Print(L"Allocated the page directory at the physical address 0x%08x\n", *DirectoryPhys);
#endif
	
	/* Map everything using big pages, we don't wanna spend a lot of memory just on the page directory,
	 * do we? */
	
	UINT32 *Directory = (UINT32*)*DirectoryPhys;
	UINTN Current = 0, FbCurrent = 0;
	
	ZeroMem((CHAR8*)Directory, 0x1000);
	
	for (; Current < *KernelEndPhys; Current += 0x400000) {
		Directory[(0xC0000000 + Current) >> 22] = (Current & ~0xFFF) | 0x83;
	}
	
	/* Map the framebuffer. */
	
	*KernelEndVirt = 0xC0000000 + Current;
	*FrameBufferVirt = 0xC0000000 + Current;
	
	for (; FbCurrent < FrameBufferSize; FbCurrent += 0x400000) {
		Directory[(0xC0000000 + Current + FbCurrent) >> 22] = ((FrameBufferPhys + FbCurrent) & ~0xFFF) | 0x83;
	}
	
	/* Finally, map some extra pages, as the kernel expects the space that the PMM will be located (which is just
	 * after the memory map) to be already mapped. */
	
	UINTN RegionCount = MaxPhysicalAddress / 0x400000;
	
	*RegionsStartPhys = *KernelEndPhys;
	*RegionsStartVirt = 0xC0000000 + Current + FbCurrent;
	
	for (; Current < *KernelEndPhys + (RegionCount * 0x84) +
									  (MaxPhysicalAddress / 0x1000) + 0x400000; Current += 0x400000) {
		Directory[(0xC0000000 + Current + FbCurrent) >> 22] = (Current & ~0xFFF) | 0x83;
	}
	
	*KernelEndPhys = Current;
	*KernelEndVirt = 0xC0000000 + Current + FbCurrent;
	
	/* Finally, create the two fixed entries: The efi_main one (so the whole system doesn't page fault when we
	 * enable paging), and the recursive one (so we can access the page tables even with paging enabled). */
	
	UINTN EfiMainAddress = (UINTN)efi_main;
	
	Directory[EfiMainAddress >> 22] = (EfiMainAddress & ~0xFFF) | 0x83;
	Directory[1023] = (*DirectoryPhys & ~0xFFF) | 0x03;
	
	return EFI_SUCCESS;
}
