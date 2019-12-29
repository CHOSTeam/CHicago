// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 23:45 BRT
// Last edited on December 26 of 2019, at 12:24 BRT

#include <chicago/alloc.h>
#include <chicago/ipc.h>
#include <chicago/shm.h>
#include <chicago/gui/gui.h>

static IntPtr GuiResponsePort = -1;

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h) {
	if (GuiResponsePort == -1) {																									// Only go forward if we are initialized
		return Null;
	}
	
	PGuiWindow window = (PGuiWindow)MmAllocMemory(sizeof(GuiWindow));																// Create the window strct
	
	if (window == Null) {
		return Null;
	}
	
	GuiCreateWindowRequest req = { x, y, w, h };																					// Create the request struct
	PIpcMessage msg = IpcSendMessage(L"GuiServer", GUI_CREATE_WINDOW_REQUEST, sizeof(req), (PUInt8)&req, GuiResponsePort);			// Send the request
	PGuiCreateWindowReply rep = (PGuiCreateWindowReply)msg->buffer;
	
	if (msg->msg == False) {
		MmFreeMemory((UIntPtr)msg);																									// Failed to create it...
		MmFreeMemory((UIntPtr)window);
		return Null;
	}
	
	UIntPtr buf = ShmMapSection(rep->shm_key);																						// Map the shared memory section
	
	if (buf == 0) {
		MmFreeMemory((UIntPtr)rep);
		MmFreeMemory((UIntPtr)msg);																									// Failed...
		MmFreeMemory((UIntPtr)window);
		return Null;
	}
	
	window->x = x;																													// Setup everthing, including the surface
	window->y = y;
	window->wkey = rep->window_key;
	window->skey = rep->shm_key;
	window->surface = ImgCreateBuf(x, y, rep->bpp, buf);
	
	MmFreeMemory((UIntPtr)rep);																										// Free some stuff that we don't need anymore
	MmFreeMemory((UIntPtr)msg);
	MmFreeMemory((UIntPtr)window);
	
	if (window->surface == Null) {
		ShmUnmapSection(window->skey);																								// Failed...
		return Null;
	}
	
	return window;
}

Void GuiInit(Void) {
	while (!IpcCheckPort(L"GuiServer")) ;																							// Wait until the gui server is initialized and avaliable
	
	while (GuiResponsePort == -1) {																									// Initialize the response port
		GuiResponsePort = IpcCreateResponsePort();
	}
}
