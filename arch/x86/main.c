// File author is Ítalo Lima Marconato Matias
//
// Created on October 24 of 2018, at 20:15 BRT
// Last edited on October 29 of 2019, at 15:58 BRT

#include "elf.h"
#include "lib.h"

EFI_FILE *FileSystemRoot = NULL;

VOID Initialize(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);									// Some forward definitions
EFI_STATUS OpenFile(EFI_FILE *Directory, EFI_FILE **Out, CHAR16 *FileName);
EFI_STATUS ReadFile(EFI_FILE *File, UINTN Offset, UINTN BufferSize, VOID *Buffer);
EFI_STATUS GetFileSize(EFI_FILE *File, UINTN *Size);
EFI_STATUS AllocatePages(UINTN Bytes, EFI_PHYSICAL_ADDRESS Memory);
EFI_STATUS CloneBootDevString(CHAR16 *Source, UINTN Size);
VOID PrintString(CHAR16 *String);
VOID WaitKeystroke(VOID);
BOOLEAN CompareMemory(VOID *m1, VOID *m2, UINTN Count);
BOOLEAN SetVideoMode(VOID);
INT32 GetMemoryMap(VOID);
VOID Jump(UINTN Entry, UINTN BootDev, UINTN MMapAddress, UINTN MMapCount, UINTN FrameBufferData, UINTN Options);

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_STATUS Status;
	EFI_EVENT Events[2];
	UINTN Index;
	EFI_INPUT_KEY Key;
	UINTN FileSize;
	EFI_FILE *Kernel, *Config;
	CHAR16 *ConfigData;
	ELF_HEADER Header;
	ELF_PHDR *PHdrs;
	UINTN Entry;
	INT32 MMapCount;
	UINTN MSize, MKey, DSize;
	UINTN BootOptions = 0x00;
	
	Initialize(ImageHandle, SystemTable);																// Initialize some stuff
	
	Status = BS->CreateEvent(EFI_EVENT_TIMER, 0, NULL, NULL, &Events[1]);								// Let's see if we should boot on verbose mode! Create a timer event
	
	if (EFI_ERROR(Status)) {
		goto c;																							// ... Just ignore and continue the boot process without checking for the F8 press
	}
	
	Status = BS->SetTimer(Events[1], TimerRelative, 1000000);											// Set the timer to 100ms of expiration
	
	if (EFI_ERROR(Status)) {
		BS->CloseEvent(Events[1]);																		// ...
	}
	
	Events[0] = ST->ConIn->WaitForKey;																	// Finish setting up the event list
	
	Status = BS->WaitForEvent(2, Events, &Index);														// Wait for the first event that we get
	
	BS->CloseEvent(Events[1]);																			// Close the timer event
	ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);															// Read the keystroke
	
	if (!EFI_ERROR(Status) && Index != 1 && Key.ScanCode == SCAN_F8) {									// F8 detected?
		BootOptions = 0x01;																				// Yes, so we should boot on verbose mode :)
	}
	
c:	Status = OpenFile(FileSystemRoot, &Config, L"\\EFI\\BOOT\\bootmgr.conf");							// Open the configuration file
	
	if (EFI_ERROR(Status)) {	
		PrintString(L"The configuration file (bootmgr.conf) is missing\r\n");							// Couldn't open it... print an error, wait for a keypress and exit
		WaitKeystroke();
		return Status;
	}
	
	Status = GetFileSize(Config, &FileSize);															// Get the config file size
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't read the configuration file (bootmgr.conf)\r\n");						// ...
		WaitKeystroke();
		return Status;
	}
	
	Status = BS->AllocatePool(EfiBootServicesData, FileSize, (VOID**)&ConfigData);						// Alloc space for reading the config file
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't read the configuration file (bootmgr.conf)\r\n");						// ...
		WaitKeystroke();
		return Status;
	}
	
	Status = ReadFile(Config, 0, FileSize, ConfigData);													// Read the config file
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't read the configuration file (bootmgr.conf)\r\n");						// ...
		BS->FreePool(ConfigData);
		WaitKeystroke();
		return Status;
	}
	
	if (!CompareMemory(ConfigData, "\xFF\xFE", 2)) {													// Check if this is a UTF-16LE file
		PrintString(L"The configuration file (bootmgr.conf) isn't UTF-16LE\r\n");						// ...
		BS->FreePool(ConfigData);
		WaitKeystroke();
		return Status;
	}
	
	if (FileSize - 2 < 2) {																				// Clone the boot dev path
		Status = CloneBootDevString(L"CdRom0", 15);
	} else {
		Status = CloneBootDevString(&ConfigData[1], (FileSize - sizeof(CHAR16)) / sizeof(CHAR16));
	}
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't copy the boot device string\r\n");										// ...
		BS->FreePool(ConfigData);
		WaitKeystroke();
		return Status;
	}
	
	BS->FreePool(ConfigData);																			// Free the readed data
	
	Status = OpenFile(FileSystemRoot, &Kernel, L"\\EFI\\BOOT\\chkrnl.elf");								// Open the kernel file
	
	if (EFI_ERROR(Status)) {
		PrintString(L"The kernel file (chkrnl.elf) is missing\r\n");									// Couldn't open it... print an error, wait for a keypress and exit
		WaitKeystroke();
		return Status;
	}
	
	Status = ReadFile(Kernel, 0, sizeof(ELF_HEADER), &Header);											// First, read the ELF header
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't read the kernel file (chkrnl.elf)\r\n");									// ...
		WaitKeystroke();
		return Status;
	} else if (!CompareMemory(Header.ident, "\177ELF", 4)) {											// Check the header magic
		PrintString(L"The kernel file (chkrnl.elf) is corrupted\r\n");									// ...
		WaitKeystroke();
		return EFI_LOAD_ERROR;
	} else if (Header.type != 2) {																		// Check if this is a ELF executable
		PrintString(L"The kernel file (chkrnl.elf) is corrupted\r\n");									// ...
		WaitKeystroke();
		return EFI_LOAD_ERROR;
	} else if (Header.machine != ELF_MACHINE) {															// Check if this file is for this architecture
		PrintString(L"The kernel file (chkrnl.elf) is corrupted\r\n");									// ...
		WaitKeystroke();
		return EFI_LOAD_ERROR;
	} else if (Header.version == 0) {																	// And if the verson isn't zero
		PrintString(L"The kernel file (chkrnl.elf) is corrupted\r\n");									// ...
		WaitKeystroke();
		return EFI_LOAD_ERROR;
	}
	
	Status = BS->AllocatePool(EfiBootServicesData, Header.ph_num * Header.ph_ent_size,
							  (VOID**)&PHdrs);															// Allocate space for reading the program headers
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't allocate memory for reading the ELF file\r\n");							// ...
		WaitKeystroke();
		return Status;
	}
	
	Status = ReadFile(Kernel, Header.ph_off, Header.ph_num * Header.ph_ent_size,
					  PHdrs);																			// Read the program headers
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't read the kernel file (chkrnl.elf)\r\n");									// ...
		WaitKeystroke();
		return Status;
	}
	
	for (UINTN i = 0; i < Header.ph_num; i++) {															// Let's load all the data and code!
		ELF_PHDR *phdr = (ELF_PHDR*)&PHdrs[i];															// Get this program header
		
		Status = AllocatePages(phdr->msize, phdr->paddr); 												// Allocate the pages at the physical address
		
		if (EFI_ERROR(Status)) {
			PrintString(L"Couldn't allocate memory for reading the ELF file\r\n");						// ...
			WaitKeystroke();
			return Status;
		}
		
		Status = ReadFile(Kernel, phdr->offset, phdr->fsize, (VOID*)phdr->paddr);						// Read the section!
		
		if (EFI_ERROR(Status)) {
			PrintString(L"Couldn't read the kernel file (chkrnl.elf)\r\n");								// ...
			WaitKeystroke();
			return Status;
		}
	}
	
	BS->FreePool(PHdrs);																				// Free the allocated space
	
	Entry = Header.entry - ELF_BASE;																	// Save the entry point
	
	if (!SetVideoMode()) {
		PrintString(L"Couldn't set any supported graphical mode\r\n");									// ...
		WaitKeystroke();
		return Status;
	}
	
	MMapCount = GetMemoryMap();																			// Finally, get the memory map
	
	if (MMapCount == -1) {
		PrintString(L"Couldn't get the memory map\r\n");												// ...
		WaitKeystroke();
		return Status;
	}
	
	BS->GetMemoryMap(&MSize, NULL, &MKey, &DSize, NULL);												// We need the MapKey
	
	Status = BS->ExitBootServices(ImageHandle, MKey);													// Call ExitBootServices!
	
	if (EFI_ERROR(Status)) {
		PrintString(L"Couldn't jump into the CHicago kernel\r\n");										// ...
		WaitKeystroke();
		return Status;
	}
	
	Jump(Entry, 0x10000, 0x3000, MMapCount, 0x2000, BootOptions);										// Jump into the kernel!
	
	return EFI_LOAD_ERROR;
}

VOID Initialize(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
	
	ST = SystemTable;																					// Let's save the system table
	BS = ST->BootServices;																				// And the boot services table
	
	BS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid,
					   (VOID**)&LoadedImage);															// Get the loaded image protocol
	BS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid,
					   (VOID**)&FileSystem);															// And the simple filesystem protocol
	
	FileSystem->OpenVolume(FileSystem, &FileSystemRoot);												// Finally, open the boot volume
}

EFI_STATUS OpenFile(EFI_FILE *Directory, EFI_FILE **Out, CHAR16 *FileName) {
	return Directory->Open(Directory, Out, FileName, EFI_FILE_MODE_READ,
						   EFI_FILE_READ_ONLY);															// Redirect to the ->Open function of the directory
}

EFI_STATUS ReadFile(EFI_FILE *File, UINTN Offset, UINTN BufferSize, VOID *Buffer) {
	File->SetPosition(File, Offset);																	// Set the file position
	return File->Read(File, &BufferSize, Buffer);														// And redirect to the ->Read function of the file
}

EFI_STATUS GetFileSize(EFI_FILE *File, UINTN *Size) {
	EFI_STATUS Status;
	UINTN FileInfoSize;
	EFI_FILE_INFO *FileInfo;
	
	File->GetInfo(File, &gEfiFileInfoGuid, &FileInfoSize, NULL);										// First, get the size of the file info struct
	Status = BS->AllocatePool(EfiBootServicesData, FileInfoSize, (VOID**)&FileInfo);					// Now, alloc space for reading the file info
	
	if (EFI_ERROR(Status)) {
		return Status;
	}
	
	Status = File->GetInfo(File, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);							// Now, get the file info!
	
	if (EFI_ERROR(Status)) {
		BS->FreePool(FileInfo);
		return Status;
	}
	
	*Size = FileInfo->FileSize;																			// Save the file size
	
	BS->FreePool(FileInfo);																				// Free the allocated space
	
	return EFI_SUCCESS;
}

EFI_STATUS AllocatePages(UINTN Bytes, EFI_PHYSICAL_ADDRESS Memory) {
	return BS->AllocatePages(AllocateAddress, EfiLoaderData,
							 (Bytes + 0x1000 - 1) / 0x1000, &Memory);									// Just call the AllocatePages function
}

EFI_STATUS CloneBootDevString(CHAR16 *Source, UINTN Size) {
	CHAR16 *Dest = (CHAR16*)0x10000;
	EFI_STATUS Status = AllocatePages((Size + 1) * sizeof(CHAR16), 0x10000);							// Allocate enough space for copying the string
	
	if (EFI_ERROR(Status)) {
		return Status;																					// Failed :(
	}
	
	for (UINTN i = 0; i < Size; i++) {																	// Copy the string
		Dest[i] = Source[i];
	}
	
	Dest[Size] = 0;																						// And end it with a zero!
	
	return EFI_SUCCESS;
}

VOID PrintString(CHAR16 *String) {
	ST->ConOut->OutputString(ST->ConOut, String);														// Just call the OutputString function
}

VOID WaitKeystroke(VOID) {
	EFI_STATUS Status = ST->ConIn->Reset(ST->ConIn, FALSE);												// First, reset the input buffer
	EFI_INPUT_KEY Key;
	
	if (EFI_ERROR(Status)) {
		return;																							// Failed... just return
	}
	
	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY) ;						// Now, just wait until the key stroke
}

BOOLEAN CompareMemory(VOID *m1, VOID *m2, UINTN Count) {
	if ((m1 == NULL) || (m2 == NULL) || (Count == 0)) {													// m1 is an NULL pointer? m2 is an NULL pointer? Zero-sized compare?
		return FALSE;																					// Yes
	}
	
	UINT8 *mp1 = m1;
	UINT8 *mp2 = m2;
	
	for (UINTN i = 0; i < Count; i++) {																	// GCC should optimize this for us :)
		if (*mp1++ != *mp2++) {
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOLEAN SetVideoMode(VOID) {
	UINTN *Data = (UINTN*)0x2000;
	EFI_HANDLE *HBuf;
	UINTN HCount, MSize, MNum;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Mode;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
	
	if (EFI_ERROR(AllocatePages(4 * sizeof(UINTN), 0x2000))) {											// First, try to allocate the pages at the physical address of the framebuffer data
		return FALSE;																					// Failed :(
	} else if (EFI_ERROR(BS->LocateHandleBuffer(ByProtocol,
											    &gEfiGraphicsOutputProtocolGuid, NULL,
										 		&HCount, &HBuf))) {										// Let's try to find the gop handle buffer
		return FALSE;																					// Failed :(
	} else if (EFI_ERROR(BS->HandleProtocol(HBuf[0],
											&gEfiGraphicsOutputProtocolGuid,
											(VOID**)&Gop))) {											// Now let's try to get the gop itself
		return FALSE;																					// Failed :(
	}
	
	for (MNum = 0; !EFI_ERROR(Gop->QueryMode(Gop, MNum, &MSize, &Mode)); MNum++) {						// Now let's try to find something with at least the minimum resolution!
		if ((Mode->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) &&
			(Mode->HorizontalResolution >= 1280) && (Mode->VerticalResolution >= 720)) {				// Found?
			break;																						// Yes!
		}
	}
	
	if ((Mode->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) ||
		(Mode->HorizontalResolution < 1280) || Mode->VerticalResolution < 720) {						// Found?
		return FALSE;																					// No...
	} else if (EFI_ERROR(Gop->SetMode(Gop, MNum))) {													// Set!
		return FALSE;																					// Failed :(
	}
	
	Data[0] = Mode->HorizontalResolution;																// Save the info
	Data[1] = Mode->VerticalResolution;
	Data[2] = 32;
	Data[3] = (UINTN)Gop->Mode->FrameBufferBase;
	
	return TRUE;
}

INT32 GetMemoryMap(VOID) {
	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN DSize = 0, Size = 0, Key = 0;
	UINT32 DVer = 0;
	EFI_STATUS Status = BS->GetMemoryMap(&Size, Map, &Key, &DSize, &DVer);								// First, let's try to get the buffer size
	
	if (Status == EFI_BUFFER_TOO_SMALL) {
		Status = BS->AllocatePool(EfiBootServicesData, Size, (VOID**)&Map);								// :) Alloc space for the mem descs
		
		if (EFI_ERROR(Status)) {
			return -1;																					// Failed...
		}
		
		Status = BS->GetMemoryMap(&Size, Map, &Key, &DSize, &DVer);										// And call BS->GetMemoryMap to get the mem descs
	}
	
	if (EFI_ERROR(Status) || Map == NULL) {
		if (Map != NULL) {																				// Free the allocated space
			BS->FreePool(Map);
		}
		
		return -1;																						// Failed...
	}
	
	Status = AllocatePages((3 * sizeof(UINTN)) * (Size / DSize), 0x3000);								// Alloc space for the memmap that we're going to give to the user
	
	if (EFI_ERROR(Status)) {
		BS->FreePool(Map);																				// Free the allocated space
		return -1;
	}
	
	for (UINTN i = 0; i < Size / DSize; i++) {															// Let's go!
		EFI_MEMORY_DESCRIPTOR *emap = (EFI_MEMORY_DESCRIPTOR*)((UINTN)Map + i * DSize);					// Get the map entry
		UINTN *cmap = (UINTN*)(0x3000 + (i * (3 * sizeof(UINTN))));
		
		*cmap++ = (UINTN)emap->PhysicalStart;															// Set the base
		*cmap++ = (UINTN)emap->NumberOfPages * 0x1000;													// Set the number of pages
		
		switch (emap->Type) {																			// And the type!
		case EfiReservedMemoryType:
		case EfiUnusableMemory:
		case EfiACPIMemoryNVS:
		case EfiMemoryMappedIO:
		case EfiMemoryMappedIOPortSpace:
		case EfiPalCode:
			*cmap++ = 0x02;																				// (Reserved)
		default:
			*cmap++ = 0x01;																				// (Free)
		}
	}
	
	BS->FreePool(Map);																					// Free the allocated space
	
	return Size / DSize;
}
