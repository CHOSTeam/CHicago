// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 16:05 BRT
// Last edited on December 24 of 2019, at 13:54 BRT

#define __DISPLAY__

#include <chicago/file.h>
#include <chicago/misc.h>

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
		SysCloseHandle(DispFrameBuffer);
		return False;
	} else if (!FsControlFile(DispFrameBuffer, 1, Null, (PUInt8)&w)) {																					// Get the width
		SysCloseHandle(DispFrameBuffer);
		return False;
	} else if (!FsControlFile(DispFrameBuffer, 2, Null, (PUInt8)&h)) {																					// And get the height
		SysCloseHandle(DispFrameBuffer);
		return False;
	}
	
	DispBackBuffer = ImgCreate(w, h, bpp);																												// Finally, create the backbuffer!
	
	if (DispBackBuffer == Null) {
		SysCloseHandle(DispFrameBuffer);																												// ... Failed
		return False;
	}
	
	ImgClear(DispBackBuffer, 0);																														// Clear the screen
	DispRefresh();
	
	return True;
}
