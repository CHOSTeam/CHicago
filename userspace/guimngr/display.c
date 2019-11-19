// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 16:05 BRT
// Last edited on November 11 of 2019, at 14:51 BRT

#define __DISPLAY__

#include <chicago/file.h>

#include <img.h>

static IntPtr DispFrameBuffer = -1;
PImage DispBackBuffer = Null;

Void DispRefresh(Void) {
	if (DispBackBuffer == Null) {																														// Make sure that we aren't running this before DispInit()
		return;
	}
	
	FsSetPosition(DispFrameBuffer, 0, 0);																												// Set the position to zero
	FsWriteFile(DispFrameBuffer, DispBackBuffer->width * DispBackBuffer->height * DispBackBuffer->bpp, (PUInt8)DispBackBuffer->buf);					// And write the backbuffer
}

Boolean DispInit(Void) {
	if (DispBackBuffer != Null) {																														// We should only be ran one time
		return False;
	}
	
	UInt32 w, h, bpp;
	
	DispFrameBuffer = FsOpenFile(L"/Devices/FrameBuffer");																								// Try to open the framebuffer device
	
	if (DispFrameBuffer == -1) {
		return False;																																	// Failed...
	} else if (!FsControlFile(DispFrameBuffer, 0, Null, (PUInt8)&bpp)) {																				// Get the bpp
		FsCloseFile(DispFrameBuffer);
		return False;
	} else if (!FsControlFile(DispFrameBuffer, 1, Null, (PUInt8)&w)) {																					// Get the width
		FsCloseFile(DispFrameBuffer);
		return False;
	} else if (!FsControlFile(DispFrameBuffer, 2, Null, (PUInt8)&h)) {																					// And get the height
		FsCloseFile(DispFrameBuffer);
		return False;
	}
	
	DispBackBuffer = ImgCreate(w, h, bpp);																												// Finally, create the backbuffer!
	
	if (DispBackBuffer == Null) {
		FsCloseFile(DispFrameBuffer);																													// ... Failed
		return False;
	}
	
	ImgClear(DispBackBuffer, 0);																														// Clear the screen
	DispRefresh();
	
	return True;
}
