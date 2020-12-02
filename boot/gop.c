/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 17 of 2020, at 19:28 BRT
 * Last edited on November 29 of 2020, at 21:55 BRT */

#include <loader.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;

EFI_STATUS InitializeGraphics(VOID) {
	/* For pretty much anything, we need the GOP, so let's try to get it. */
	
	EFI_STATUS Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**)&Gop);
	
	if (EFI_ERROR(Status)) {
		Print(L"EFI Loader Error: Couldn't get the GOP: %r\n", Status);
		return Status;
	}
	
	/* Let's try to find the best mode for us. */
	
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Mode;
	UINTN ModeSize = 0, CurrentMode = Gop->Mode->Mode;
	UINTN ModeWidth = Gop->Mode->Info->HorizontalResolution;
	UINTN ModeHeight = Gop->Mode->Info->VerticalResolution;
	
	for (UINTN ModeNum = 0; ModeNum < Gop->Mode->MaxMode; ModeNum++) {
		/* Check rules: If it's the current selected mode, ignore.
		 *				If the QueryMode function fails, ignore.
		 *				If the resolution is lower than the highest resolution that we found, ignore.
		 *				If the resolution is bigger than 1280x720, ignore.
		 *				If the pixel format isn't BGRA, ignore. */
		
		if (ModeNum == CurrentMode ||
			EFI_ERROR((Status = Gop->QueryMode(Gop, ModeNum, &ModeSize, &Mode))) ||
			(Mode->HorizontalResolution < ModeWidth || Mode->VerticalResolution < ModeHeight) ||
			(Mode->HorizontalResolution > 1280 || Mode->VerticalResolution > 720) ||
			Mode->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
			continue;
		}
		
		CurrentMode = ModeNum;
		ModeWidth = Mode->HorizontalResolution;
		ModeHeight = Mode->VerticalResolution;
	}
	
	/* Now, let's set the mode, even if it's the old selected one. */
	
	if (EFI_ERROR((Status = Gop->SetMode(Gop, CurrentMode)))) {
		Print(L"EFI Loader Error: Couldn't set the resolution: %r\n", Status);
		return Status;
	}
	
	/* If we were compiled with debugging enabled, print some info.
	 * Uncomment lines 62 and 63 when we discover what the heck is going wrong on that Print(). */
	
#ifndef _NDEBUG
	Print(L"CHicago x86 EFI Loader\n");
	Print(L"Version %s\n", VERSION);
	Print(L"Current Video Mode Information\n");
	Print(L"    Number: %d\n", Gop->Mode->Mode);
	Print(L"    Resolution: %dx%d\n",
		  Gop->Mode->Info->HorizontalResolution, Gop->Mode->Info->VerticalResolution);
#ifdef _WIN64
	Print(L"    Framebuffer: 0x%llx\n", Gop->Mode->FrameBufferBase);
	Print(L"    Framebuffer Size: 0x%llx\n", Gop->Mode->FrameBufferSize);
#else
	Print(L"    Framebuffer: 0x%x\n", Gop->Mode->FrameBufferBase);
	Print(L"    Framebuffer Size: 0x%x\n", Gop->Mode->FrameBufferSize);
#endif
#endif
	
	return Status;
}
