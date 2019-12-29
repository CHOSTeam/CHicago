// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 21:49 BRT
// Last edited on December 25 of 2019, at 20:48 BRT

#include <chicago/alloc.h>

#include <display.h>
#include <window.h>

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h) {
	PGuiWindow window = (PGuiWindow)MmAllocMemory(sizeof(GuiWindow));									// Try to alloc memory for the window struct
	
	if (window == Null) {
		return Null;																					// Failed...
	}
	
	window->surface = ImgCreate(w, h, DispBackBuffer != Null ? DispBackBuffer->bpp : 0);				// Create our surface
	
	if (window->surface == Null) {
		MmFreeMemory((UIntPtr)window);																	// Failed...
		return Null;
	}
	
	ImgFillRectangle(window->surface, 0, 0, w, h, 0xFFEBEBE4);											// Clear it
	
	window->x = x;																						// And setup the rest of the struct
	window->y = y;
	window->key = 0;
	
	return window;
}

PGuiWindow GuiCreateWindowBuf(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr buf) {
	PGuiWindow window = (PGuiWindow)MmAllocMemory(sizeof(GuiWindow));									// Try to alloc memory for the window struct
	
	if (window == Null) {
		return Null;																					// Failed...
	}
	
	window->surface = ImgCreateBuf(w, h, DispBackBuffer != Null ? DispBackBuffer->bpp : 0, buf);		// Create our surface
	
	if (window->surface == Null) {
		MmFreeMemory((UIntPtr)window);																	// Failed...
		return Null;
	}
	
	ImgFillRectangle(window->surface, 0, 0, w, h, 0xFFEBEBE4);											// Clear it
	
	window->x = x;																						// And setup the rest of the struct
	window->y = y;
	window->key = 0;
	
	return window;
}

Void GuiFreeWindow(PGuiWindow window) {
	if (window == Null) {																				// As always, first let's do the null pointer check
		return;
	}
	
	MmFreeMemory((UIntPtr)window->surface);																// And let's free everything!
	MmFreeMemory((UIntPtr)window);
}
