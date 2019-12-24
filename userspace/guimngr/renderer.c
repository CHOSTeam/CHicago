// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 21:41 BRT
// Last edited on December 24 of 2019, at 13:54 BRT

#include <chicago/alloc.h>
#include <chicago/list.h>
#include <chicago/misc.h>
#include <chicago/process.h>

#include <display.h>
#include <renderer.h>

static PImage RendererTheme = Null;
static IntPtr GuiRefreshLock = -1;
static Volatile Boolean GuiShouldRefresh = True;
static List GuiWindowList = { Null, Null, 0, False };

static Void GuiBitBlit(UIntPtr sx, UIntPtr sy, UIntPtr dx, UIntPtr dy, UIntPtr w, UIntPtr h) {
	ImgBitBlit(DispBackBuffer, RendererTheme, sx, sy, dx, dy, w, h, BITBLIT_MODE_COPY);																// Just redirect to ImgBitBlit using the DispBackBuffer as the first arg
}

static Void GuiBitBlitRescale(UIntPtr sx, UIntPtr sy, UIntPtr sw, UIntPtr sh, UIntPtr dx, UIntPtr dy, UIntPtr dw, UIntPtr dh) {
	PImage img = ImgCreate(sw, sh, RendererTheme->bpp);																								// Try to create the img struct
	
	if (img == Null) {
		return;
	}
	
	ImgBitBlit(img, RendererTheme, sx, sy, 0, 0, sw, sh, BITBLIT_MODE_COPY);																		// Copy the image
	
	PImage res = ImgRescale(img, dw, dh);																											// Try to rescale
	
	if (res == Null) {
		MmFreeMemory((UIntPtr)img);																													// Failed...
	}
	
	ImgBitBlit(DispBackBuffer, res, 0, 0, dx, dy, dw, dh, BITBLIT_MODE_COPY);																		// And BitBlit into the backbuffer
	MmFreeMemory((UIntPtr)res);																														// And free everything
	MmFreeMemory((UIntPtr)img);
}

static Void RendererMainThread(Void) {
	while (True) {																																	// Enter the render loop!
		while (!GuiShouldRefresh) ;																													// Wait until we need to do something
		
		PsLock(GuiRefreshLock);																														// Lock
		ImgFillRectangle(DispBackBuffer, 0, 0, DispBackBuffer->width, DispBackBuffer->height, 0xFF000000);											// Clean the screen
		
		ListForeach(&GuiWindowList, i) {																											// Let's iterate the window list!
			PGuiWindow window = (PGuiWindow)i->data;																								// Get the window struct
			PImage surface = window->surface;																										// Save the surface
			Boolean top = i->next == Null;																											// Save if this window is in the top
			
			if (top) {																																// Now, let's draw the frame, this one is in the top?
				GuiBitBlit(0, 0, window->x, window->y, 4, 23);																						// Yes!
				GuiBitBlit(8, 0, window->x + surface->width + 4, window->y, 4, 23);
				GuiBitBlit(0, 29, window->x, window->y + surface->height + 23, 4, 4);
				GuiBitBlit(8, 29, window->x + surface->width + 4, window->y + surface->height + 23, 4, 4);
				GuiBitBlitRescale(4, 0, 4, 23, window->x + 4, window->y, surface->width, 23);
				GuiBitBlitRescale(4, 29, 4, 4, window->x + 4, window->y + surface->height + 23, surface->width, 4);
				GuiBitBlitRescale(0, 23, 4, 6, window->x, window->y + 23, 4, surface->height);
				GuiBitBlitRescale(8, 23, 4, 6, window->x + surface->width + 4, window->y + 23, 4, surface->height);
			} else {
				GuiBitBlit(12, 0, window->x, window->y, 4, 23);																						// Nope :(
				GuiBitBlit(20, 0, window->x + surface->width + 4, window->y, 4, 23);
				GuiBitBlit(12, 29, window->x, window->y + surface->height + 23, 4, 4);
				GuiBitBlit(20, 29, window->x + surface->width + 4, window->y + surface->height + 23, 4, 4);
				GuiBitBlitRescale(16, 0, 4, 23, window->x + 4, window->y, surface->width, 23);
				GuiBitBlitRescale(16, 29, 4, 4, window->x + 4, window->y + surface->height + 23, surface->width, 4);
				GuiBitBlitRescale(12, 23, 4, 6, window->x, window->y + 23, 4, surface->height);
				GuiBitBlitRescale(20, 23, 4, 6, window->x + surface->width + 4, window->y + 23, 4, surface->height);
			}
			
			ImgBitBlit(DispBackBuffer, surface, 0, 0, window->x + 4, window->y + 23, surface->width, surface->height, BITBLIT_MODE_COPY);			// And draw the user surface!
		}
		
		GuiShouldRefresh = False;																													// Ok, now unset the refresh flag
		
		DispRefresh();																																// Copy everything into the framebuffer
		PsUnlock(GuiRefreshLock);																													// And unlock
	}
}

Boolean RendererLoadTheme(PWChar path) {
	if (path == Null) {																																// First, sanity check
		return False;
	}
	
	PImage theme = ImgLoadBMP(path);																												// Now, let's try to load the theme BMP
	
	if (theme == Null) {
		return False;																																// Failed, probably it doesn't exists
	} else if (RendererTheme != Null) {																												// Do we need to free the old loaded one?
		MmFreeMemory((UIntPtr)RendererTheme);																										// Yes
	}
	
	RendererTheme = theme;																															// Set the new one!
	
	return True;
}

static Boolean GuiGetWindow(PGuiWindow window, PUIntPtr out) {
	if (window == Null) {																															// Sanity check
		return False;
	}
	
	UIntPtr idx = 0;																																// Now, let's iterate the window list
	
	ListForeach(&GuiWindowList, i) {
		if (window == i->data) {																													// Check if it is what we are searching
			if (out != Null) {																														// Yes, it is! Save the idx if we need to
				*out = idx;
			}
			
			return True;
		}
		
		idx++;
	}
	
	return False;
}

Void GuiAddWindow(PGuiWindow window) {
	if (window == Null) {																															// Sanity check
		return;
	} else if (GuiGetWindow(window, Null) || !ListAdd(&GuiWindowList, window)) {																	// Check if it's not already added, and add it!
		return;
	} else {
		GuiRefresh();																																// REFRESH!
	}
}

Void GuiRemoveWindow(PGuiWindow window) {
	if (window == Null) {																															// Sanity check
		return;
	}
	
	UIntPtr idx = 0;
	
	if (!GuiGetWindow(window, &idx)) {																												// Get the index of the window in the window list
		return;
	} else if (ListRemove(&GuiWindowList, idx) != Null) {																							// Remove it
		GuiRefresh();																																// And refresh!
	}
}

Void GuiRefresh(Void) {
	PsLock(GuiRefreshLock);																															// Lock
	GuiShouldRefresh = True;																														// Set that we should refresh
	PsUnlock(GuiRefreshLock);																														// Unlock
}

Boolean RendererInit(Void) {
	GuiRefreshLock = PsCreateLock();																												// Create the refresh lock
	
	if (GuiRefreshLock == -1) {
		return False;
	} else if (!RendererLoadTheme(L"/System/Themes/Blue.theme")) {																					// Load the default theme
		SysCloseHandle(GuiRefreshLock);
		return False;
	}
	
	return PsCreateThread((UIntPtr)RendererMainThread) != 0;																						// And create the renderer thread!
}
