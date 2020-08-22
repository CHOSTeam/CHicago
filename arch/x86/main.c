/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 17 of 2020, at 19:27 BRT
 * Last edited on August 01 of 2020, at 22:07 BRT */

#include <stddef.h>
#include <stdint.h>
#include <sia.h>

EFI_HANDLE *EspHandle;

static BOOLEAN IsPciRoot(EFI_DEVICE_PATH *DevicePath) {
	/* Btw, PciRoot(%d) is the same as ACPI(PNP0A03,%d). */
	
	if (DevicePath->Type != ACPI_DEVICE_PATH || DevicePath->SubType != ACPI_DP) {
		return FALSE;
	} else if (!(((ACPI_HID_DEVICE_PATH*)DevicePath)->HID & PNP_EISA_ID_MASK)) {
		return FALSE;
	}
	
	return EISA_ID_TO_NUM(((ACPI_HID_DEVICE_PATH*)DevicePath)->HID) == 0xA03;
}

static VOID InitFPU(VOID) {
	UINT16 cw0 = 0x37E, cw1 = 0x37A;
	UINTN cr0, cr4;
	
	/* There are some specific bits that we need to set at CR0 and CR4, to make sure that SSE will be enabled.
	 * Those bits are: the EMulation bit, the Monitor co-Processor bit, the FXsave/fxrStoR bits, and also set
	 * that the OS will handle unmasked SIMD exceptions. */
	
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
	__asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4));
	
	cr0 &= ~(1 << 2);
	cr0 |= 1 << 1;
	cr4 |= (1 << 9) | (1 << 10);
	
	__asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
	__asm__ __volatile__("mov %0, %%cr4" :: "r"(cr4));
	
	/* Now, let's setup the default FPU state, and make sure that both division-by-zero and invalid operands
	 * will throw exceptions. */
	
	__asm__ __volatile__("fninit");
	__asm__ __volatile__("fldcw %0" :: "m"(cw0));
	__asm__ __volatile__("fldcw %0" :: "m"(cw1));
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	/* GNU-EFI gives us a pretty complete library for printing to the screen, allocating memory and etc, so,
	 * let's initialize that library and clear the screen. */
	
	InitializeLib(ImageHandle, SystemTable);
	gST->ConOut->ClearScreen(gST->ConOut);
	
	/* Disable the watchdog timer, else, we may not have enough time to load everything (not that this is
	 * going to happen for now, but, if the kernel grow too big, or, if we add IPXE support, it may happen). */
	
	gBS->SetWatchdogTimer(0, 0, 0, NULL);
	
	/* Initialize the graphics (try to set the highest possible mode, or, at least, get info about the
	 * current mode. */
	
	if (EFI_ERROR(InitializeGraphics())) {
		goto e;
	}
	
	/* Get the boot device ESP handle, the ReadFile function needs it, and also we're going to use it later to
	 * find the boot device/create a "description" that the kernel can understand. */
	
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	
	if (EFI_ERROR(gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage))) {
		Print(L"EFI Loader Error: Couldn't get the boot device ESP handle.\n");
		goto e;
	}
	
	EspHandle = LoadedImage->DeviceHandle;
	
	/* Open the initrd/kernel file (BOOT.SIA), it should be located in the EFI\CHOS directory on the ESP. */
	
	CHAR8 *Buffer;
	UINTN Size;
	
	if (EFI_ERROR(ReadFile(L"\\EFI\\CHOS\\BOOT.SIA", &Buffer, &Size))) {
		goto e;
	}
	
	/* Now, let's check if the SIA file is valid, and finally, let's actually load the kernel file. */
	
	UINTN KernelEntry = 0, KernelStartPhys = UINTPTR_MAX, KernelStartVirt = UINTPTR_MAX,
		  KernelEndPhys = 0, KernelEndVirt = 0;
	
	if (EFI_ERROR(SiaLoadKernel(Buffer, Size, &KernelEntry, &KernelStartPhys, &KernelStartVirt,
								&KernelEndPhys, &KernelEndVirt))) {
		goto e;
	}
	
	/* Before jumping into the kernel, we need to make the boot info struct (the kernel expects it as the first argument.
	 * Allocate it just after the kernel itself. */
	
	EFI_PHYSICAL_ADDRESS PhysLocation = KernelEndPhys;
	UINTN BootInfoVirt = KernelEndVirt;
	
	KernelEndPhys += ((sizeof(BOOT_INFO) + 0xFFF) / 0x1000) * 0x1000;
	KernelEndVirt += ((sizeof(BOOT_INFO) + 0xFFF) / 0x1000) * 0x1000;
	
	if (EFI_ERROR(gBS->AllocatePages(AllocateAddress, EfiLoaderData,
									 (sizeof(BOOT_INFO) + 0xFFF) / 0x1000, &PhysLocation))) {
		Print(L"EFI Loader Error: Couldn't allocate memory for the boot information struct.\n");
		goto e;
	}
	
	ZeroMem((CHAR8*)PhysLocation, ((sizeof(BOOT_INFO) + 0xFFF) / 0x1000) + 0x1000);
	
	BOOT_INFO *BootInfo = (BOOT_INFO*)PhysLocation;
	
#ifdef DBG
#ifdef ARCH_64
	Print(L"Allocated the boot info struct at the physical address 0x%llx (virtual address 0x%llx)\n",
		  BootInfo, BootInfoVirt);
#else
	Print(L"Allocated the boot info struct at the physical address 0x%x (virtual address 0x%x)\n",
		  BootInfo, BootInfoVirt);
#endif
#endif
	
	/* Relloc the BOOT.SIA file contents (which are currently at the Buffer variable). */
	
	CHAR8 *NewBuffer = (CHAR8*)KernelEndVirt;
	
	PhysLocation = KernelEndPhys;
	KernelEndPhys += ((Size + 0xFFF) / 0x1000) * 0x1000;
	KernelEndVirt += ((Size + 0xFFF) / 0x1000) * 0x1000;
	
	if (EFI_ERROR(gBS->AllocatePages(AllocateAddress, EfiLoaderCode,
									 (Size + 0xFFF) / 0x1000, &PhysLocation))) {
		Print(L"EFI Loader Error: Couldn't allocate memory for relocating the kernel SIA file.\n");
		goto e;
	}
	
	ZeroMem((CHAR8*)PhysLocation, ((Size + 0xFFF) / 0x1000) * 0x1000);
	CopyMem((CHAR8*)PhysLocation, Buffer, Size);
	
#ifdef DBG
#ifdef ARCH_64
	Print(L"Relocated the kernel SIA file to the physical address 0x%llx (virtual address 0x%llx)\n",
		  PhysLocation, NewBuffer);
#else
	Print(L"Relocated the kernel SIA file to the physical address 0x%x (virtual address 0x%x)\n",
		  PhysLocation, NewBuffer);
#endif
#endif
	
	/* Finally, start filling the boot info struct! */
	
	BootInfo->Magic = BOOT_INFO_MAGIC;
	BootInfo->KernelStart = KernelStartPhys;
	BootInfo->KernelStartVirt = (UINT8*)KernelStartVirt;
	BootInfo->EfiMainAddress = (UINTN)efi_main;
	BootInfo->BootSIA.Size = Size;
	BootInfo->BootSIA.Contents = NewBuffer;
	BootInfo->FrameBuffer.Width = Gop->Mode->Info->HorizontalResolution;
	BootInfo->FrameBuffer.Height = Gop->Mode->Info->VerticalResolution;
	BootInfo->FrameBuffer.Size = Gop->Mode->FrameBufferSize;
	
	/* Time to get the boot device, for this, we can use the ESP handle we extracted before, with the device path
	 * (of the ESP), we can get the PCI bus/slot/func and some more specific info about the device. */
	
	EFI_DEVICE_PATH *DevicePath = DevicePathFromHandle(EspHandle);
	UINT8 Found = 0;
	
	if (DevicePath == NULL) {
		Print(L"EFI Loader Error: Couldn't get the boot device.\n");
		goto e;
	}
	
	for (; !IsDevicePathEnd(DevicePath) && Found != 3; DevicePath = NextDevicePathNode(DevicePath)) {
		/* We only support (P)ATA and SATA for now.
		 * First, we try to get the info about the PCI port that the device is connected.
		 * After that, get more specific info that changes depending on the device type. */
		
		if (!Found && !IsPciRoot(DevicePath)) {
			continue;
		} else if (Found == 1 &&
				   (DevicePath->Type != HARDWARE_DEVICE_PATH || DevicePath->SubType != HW_PCI_DP)) {
			continue;
		} else if (Found == 2 && DevicePath->Type != MESSAGING_DEVICE_PATH &&
				   (DevicePath->SubType != MSG_ATAPI_DP || DevicePath->SubType != MSG_SATA_DP)) {
			continue;
		} else if (!Found) {
			BootInfo->BootDevice.PCI.Bus = ((ACPI_HID_DEVICE_PATH*)DevicePath)->UID;
			Found++;
			
#ifdef DBG
			Print(L"Boot Device Information\n");
			Print(L"    PCI Bus: %d\n", BootInfo->BootDevice.PCI.Bus);
#endif
			continue;
		} else if (Found == 1) {
			BootInfo->BootDevice.PCI.Device = ((PCI_DEVICE_PATH*)DevicePath)->Device;
			BootInfo->BootDevice.PCI.Function = ((PCI_DEVICE_PATH*)DevicePath)->Function;
			Found++;
			
#ifdef DBG
			Print(L"    PCI Device: %d\n", BootInfo->BootDevice.PCI.Device);
			Print(L"    PCI Function: %d\n", BootInfo->BootDevice.PCI.Function);
#endif
			
			continue;
		}
		
		if (DevicePath->SubType == MSG_ATAPI_DP) {
			BootInfo->BootDevice.Type = BOOT_INFO_DEV_ATA;
			BootInfo->BootDevice.ATA.Primary = !((ATAPI_DEVICE_PATH*)DevicePath)->PrimarySecondary;
			BootInfo->BootDevice.ATA.Master = !((ATAPI_DEVICE_PATH*)DevicePath)->SlaveMaster;
			
#ifdef DBG
			Print(L"    ATA\n");
			Print(L"        %s\n", BootInfo->BootDevice.ATA.Primary ? L"Primary" : L"Secondary");
			Print(L"        %s\n", BootInfo->BootDevice.ATA.Master ? L"Master" : L"Slave");
#endif
		} else if (DevicePath->SubType == MSG_SATA_DP) {
			BootInfo->BootDevice.Type = BOOT_INFO_DEV_SATA;
			BootInfo->BootDevice.SATA.HBAPort = ((SATA_DEVICE_PATH*)DevicePath)->HBAPortNumber;
			BootInfo->BootDevice.SATA.Mult = ((SATA_DEVICE_PATH*)DevicePath)->PortMultiplierPortNumber;
			BootInfo->BootDevice.SATA.Lun = ((SATA_DEVICE_PATH*)DevicePath)->Lun;
			
#ifdef DBG
			Print(L"    SATA\n");
			Print(L"        HBA Port: %d\n", BootInfo->BootDevice.SATA.HBAPort);
			Print(L"        Port Multiplier: %d\n", BootInfo->BootDevice.SATA.Mult);
			Print(L"        Lun: %d\n", BootInfo->BootDevice.SATA.Lun);
#endif
		}
		
		Found++;
	}
	
	if (Found != 3) {
		Print(L"EFI Loader Error: This boot device isn't supported.\n");
		goto e;
	}
	
	/* Allocate space for the memory map that the kernel will have access, and fill it using the EFI memory map. */
	
	UINTN DescSize = 0, MapCount = 0, MapKey = 0, MapSize = 0, MaxAddress = 0, MaxSize = 0;
	UINT32 DescVer = 0;
	EFI_MEMORY_DESCRIPTOR *Map = LibMemoryMap(&MapCount, &MapKey, &DescSize, &DescVer);
	
	if (Map == NULL) {
		Print(L"EFI Loader Error: Couldn't get the memory map.\n");
	}
	
	PhysLocation = KernelEndPhys;
	BootInfo->MemoryMap.EntryCount = MapCount;
	BootInfo->MemoryMap.Entries = (BOOT_INFO_MEM_MAP*)KernelEndVirt;
	
	BOOT_INFO_MEM_MAP *MemoryMap = (BOOT_INFO_MEM_MAP*)PhysLocation;
	UINTN MemMapPages = (sizeof(BOOT_INFO_MEM_MAP) * MapCount + 0xFFF) / 0x1000;
	UINTN MemMapSize = MemMapPages * 0x1000;
	
	if (EFI_ERROR(gBS->AllocatePages(AllocateAddress, EfiLoaderData, MemMapPages, &PhysLocation))) {
		Print(L"EFI Loader Error: Couldn't allocate memory for the memory map.\n");
		goto e;
	}
	
	ZeroMem(MemoryMap, MemMapSize);
	
	KernelEndPhys += MemMapSize;
	KernelEndVirt += MemMapSize;
	
#ifdef DBG
#ifdef ARCH_64
	Print(L"Allocated the memory map at the physical address 0x%llx (virtual address 0x%llx)\n",
		  MemoryMap, BootInfo->MemoryMap.Entries);
#else
	Print(L"Allocated the memory map at the physical address 0x%x (virtual address 0x%x)\n",
		  MemoryMap, BootInfo->MemoryMap.Entries);
#endif
#endif
	
	for (UINTN i = 0; i < MapCount; i++) {
		EFI_MEMORY_DESCRIPTOR *EfiDesc = (EFI_MEMORY_DESCRIPTOR*)((UINTN)Map + i * DescSize);
		
		MemoryMap[i].Base = (UINTN)EfiDesc->PhysicalStart;
		MemoryMap[i].Size = (UINTN)EfiDesc->NumberOfPages * 0x1000;
		
		/* If this is higher than the old max addressable address, update it. */
		
		if (MemoryMap[i].Base + MemoryMap[i].Size > MaxAddress) {
			MaxAddress = MemoryMap[i].Base + MemoryMap[i].Size;
		}
		
		/* Any memory that is already being used by some device, is straight up reserved or unused, or is being
		 * used by the firmware/motherboard is out of reach for us. */
		
		switch (EfiDesc->Type) {
		case EfiReservedMemoryType:
		case EfiUnusableMemory:
		case EfiACPIMemoryNVS:
		case EfiMemoryMappedIO:
		case EfiMemoryMappedIOPortSpace:
		case EfiPalCode:
			MemoryMap[i].Free = FALSE;
			break;
		default:
			/* Valid and usable memory, we need to increase our usable memory counter. */
			
			MaxSize += MemoryMap[i].Size;
			MemoryMap[i].Free = TRUE;
			
			break;
		}
	}
	
	FreePool(Map);
	
	/* Finally, create the initial kernel page directory, and fill in the boot info struct the directory physical
	 * and virtual address. */
	
	UINTN DirectoryPhys, FrameBufferVirt, RegionsPhys, RegionsVirt;
	
	if (EFI_ERROR(CreateDirectory(&DirectoryPhys, &KernelEndPhys, &KernelEndVirt,
								  Gop->Mode->FrameBufferBase, &FrameBufferVirt, Gop->Mode->FrameBufferSize,
								  MaxAddress, &RegionsPhys, &RegionsVirt))) {
		goto e;
	}
	
	BootInfo->Directory = DirectoryPhys;
	BootInfo->MaxPhysicalAddress = MaxAddress;
	BootInfo->PhysicalMemorySize = MaxSize;
	BootInfo->RegionsStart = RegionsPhys;
	BootInfo->RegionsStartVirt = (UINT8*)RegionsVirt;
	BootInfo->FrameBuffer.Address = FrameBufferVirt;
	
#ifdef DBG
#ifdef ARCH_64
	Print(L"Reserved the virtual address 0x%llx for the framebuffer.\n", FrameBufferVirt);
#else
	Print(L"Reserved the virtual address 0x%x for the framebuffer.\n", FrameBufferVirt);
#endif
#endif
	
	/* Before going into the kernel, we still have two things we have to do:
	 * Finish the boot info struct by putting the kernel end address, we couldn't do it until now as we were
	 * still doing new allocations and changing this value.
	 * Use the ExitBootServices function to get out of the EFI environment, and be safe to jump into the kernel. */
	
	BootInfo->KernelEnd = KernelEndPhys;
	BootInfo->KernelEndVirt = (UINT8*)KernelEndVirt;
	
#ifdef DBG
#ifdef ARCH_64
	Print(L"The kernel end is at the physical address 0x%llx (virtual address 0x%llx)\n",
		  KernelEndPhys, KernelEndVirt);
#else
	Print(L"The kernel end is at the physical address 0x%x (virtual address 0x%x)\n",
		  KernelEndPhys, KernelEndVirt);
#endif
#endif
	
	/* Now let's use ExitBootServices, this is going to put us out of the EFI environment, and we'll be ready to jump into
	 * the kernel. */
	
	MapSize = MapKey = DescSize = 0;
	
	gBS->GetMemoryMap(&MapSize, NULL, &MapKey, &DescSize, NULL);
	
	if (EFI_ERROR(gBS->ExitBootServices(ImageHandle, MapKey))) {
		Print(L"EFI Loader Error: Can't exit the EFI environment.\n");
		goto e;
	}
	
	/* Setup the FPU, disable interrupts, else, we're going to get some recursive page faults on any interrupt, enable paging,
	 * and jump into the kernel entry point (which is not the kernel main function btw). */
	
	InitFPU();
	
	__asm__ __volatile__("cli");
	
#ifdef ARCH_64
	__asm__ __volatile__("mov %0, %%rbx; mov %1, %%rdi" ::
						 "r"(KernelEntry), "r"(BootInfoVirt) : "%rbx", "%rdi");
	__asm__ __volatile__("mov %0, %%cr3" :: "a"(DirectoryPhys));
	__asm__ __volatile__("mov %0, %%rsp; add %%rdi, %%rsp" ::
						 "a"(((offsetof(BOOT_INFO, KernelStack) + sizeof(BootInfo->KernelStack)) & -16) + 16));
	__asm__ __volatile__("call *%rbx");
#else
	__asm__ __volatile__("mov %0, %%ebx; mov %1, %%ecx" ::
						 "r"(BootInfoVirt), "r"(KernelEntry) : "%ebx", "%ecx");
	__asm__ __volatile__("mov %0, %%cr3" :: "a"(DirectoryPhys));
	__asm__ __volatile__("mov %cr4, %eax; or $0x10, %eax; mov %eax, %cr4");
	__asm__ __volatile__("mov %cr0, %eax; or $0x80010001, %eax; mov %eax, %cr0");
	__asm__ __volatile__("mov %0, %%esp; add %%ebx, %%esp" ::
						 "a"(((offsetof(BOOT_INFO, KernelStack) + sizeof(BootInfo->KernelStack)) & -8) + 12));
	__asm__ __volatile__("push %ebx; call *%ecx");
#endif
	
e:	while (TRUE) ;
}
