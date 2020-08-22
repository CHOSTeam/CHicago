/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 20 of 2020, at 13:00 BRT
 * Last edited on June 25 of 2020, at 07:50 BRT */

#include <loader.h>

EFI_FILE *FsRoot = NULL;

EFI_STATUS ReadFile(CHAR16 *FileName, CHAR8 **Buffer, UINTN *Size) {
	/* First, we need to init the FsRoot variable if it's the first time we're calling this function. */
	
	EFI_STATUS Status;
	
	if (FsRoot == NULL && (FsRoot = LibOpenRoot(EspHandle)) == NULL) {
		Print(L"EFI Loader Error: Failed to initialize the filesystem\n");
		return EFI_LOAD_ERROR;
	}
	
	/* Time to try to open the file, if the open function fails, probably the file don't exist. */
	
	EFI_FILE *File;
	
	if (EFI_ERROR((Status = FsRoot->Open(FsRoot, &File, FileName,
										 EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY)))) {
		Print(L"EFI Loader Error: Failed to open '%s'\n", FileName);
		return Status;
	}
	
	/* Time to the the file info struct, to do it, we could use two GetInfo calls, but, as we're using
	 * GNU-EFI, we can just use a single LibFileInfo call! */
	
	EFI_FILE_INFO *Info = LibFileInfo(File);
	
	if (Info == NULL) {
		Print(L"EFI Loader Error: Couldn't read '%s' size\n");
		File->Close(File);
		return Status;
	}
	
#ifdef DBG
	Print(L"Opened the file '%s'\n", FileName);
	Print(L"    Size: %d bytes\n", Info->FileSize);
	Print(L"    Physical Size: %d bytes\n", Info->PhysicalSize);
	Print(L"    Creation Time: %t\n", &Info->CreateTime);
	Print(L"    Last Access Time: %t\n", &Info->LastAccessTime);
	Print(L"    Last Modification Time: %t\n", &Info->ModificationTime);
#endif
	
	/* Save the size on the Size argument, and free the file info struct (yes, we only need it to get
	 * the size, and print some info when compiled with the DBG flag). */
	
	*Size = Info->FileSize;
	FreePool(Info);
	
	/* Finally, allocate space using AllocatePages and read the file! */
	
	UINTN BufferSize = (*Size + 0xFFF) & -0x1000;
	EFI_PHYSICAL_ADDRESS Location;
	
	if (EFI_ERROR((Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, BufferSize / 0x1000, &Location)))) {
		Print(L"EFI Loader Error: Couldn't allocate memory for reading '%s'\n", FileName);
		File->Close(File);
		return Status;
	} else if (EFI_ERROR((Status = File->Read(File, Size, (VOID*)Location)))) {
		Print(L"EFI Loader Error: Couldn't read '%s'\n", FileName);
		gBS->FreePages(Location, BufferSize);
#ifdef DBG
	} else {
#ifdef ARCH_64
		Print(L"    Load Location: 0x%016llx\n", Location);
#else
		Print(L"    Load Location: 0x%08x\n", Location);
#endif
#endif
	}
	
	*Buffer = (CHAR8*)Location;
	
	/* Close the file, and return! */
	
	File->Close(File);
	
	return Status;
}
