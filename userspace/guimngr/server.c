// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 19:53 BRT
// Last edited on December 29 of 2019, at 18:08 BRT

#include <chicago/alloc.h>
#include <chicago/ipc.h>
#include <chicago/misc.h>
#include <chicago/process.h>
#include <chicago/shm.h>

#include <display.h>
#include <renderer.h>
#include <server.h>
#include <window.h>

static Void GuiServerListener(Void) {
	while (True) {
		PIpcMessage msg = IpcReceiveMessage(L"GuiServer");																			// Listen for messages!
		
		if (msg->msg == GUI_CREATE_WINDOW_REQUEST && msg->rport != -1) {															// Create window request? If yes, check if the rport is set
			PGuiCreateWindowRequest req = (PGuiCreateWindowRequest)msg->buffer;														// Get the request data
			UIntPtr shmkey;
			UIntPtr buf = ShmCreateSection(req->w * req->h * (DispBackBuffer != Null ? DispBackBuffer->bpp : 0), &shmkey);			// Create the shm section
			
			if (buf == 0) {
				IpcSendResponse(msg->rport, False, 0, Null);																		// Failed...
				goto e;
			}
			
			PGuiWindow window = GuiCreateWindowBuf(req->x, req->y, req->w, req->h, buf);											// Create the window
			
			if (window == Null) {
				ShmUnmapSection(shmkey);																							// Failed...
				IpcSendResponse(msg->rport, False, 0, Null);
				goto e;
			}
			
			PGuiCreateWindowReply rep = (PGuiCreateWindowReply)MmAllocMemory(sizeof(GuiCreateWindowReply));							// Let's create the reply
			
			if (rep == Null) {
				GuiFreeWindow(window);																								// Failed...
				ShmUnmapSection(shmkey);
				IpcSendResponse(msg->rport, False, 0, Null);
				goto e;
			}
			
			GuiAddWindow(window);																									// Add the window (also setup the window key)
			
			rep->shm_key = shmkey;																									// Setup the response
			rep->window_key = window->key;
			rep->bpp = DispBackBuffer != Null ? DispBackBuffer->bpp : 0;
			
			IpcSendResponse(msg->rport, True, sizeof(GuiCreateWindowReply), (PUInt8)rep);											// And send it!
			MmFreeMemory((UIntPtr)rep);
		} else if (msg->msg == GUI_REMOVE_WINDOW_REQUEST) {																			// Remove window request?
			GuiRemoveWindow(GuiGetWindow(msg->size));																				// Yes, try to get the window (the key stored in the ->size prop of the message) and pass it to GuiRemoveWindow (it will handle Null paramenters)
		} else if (msg->msg == GUI_REFRESH_REQUEST) {																				// Refresh request?
			GuiRefresh();																											// Yes, just call GuiRefresh()
		}
		
e:		if (msg->buffer != Null) {																									// Free the msg buffer
			MmFreeMemory((UIntPtr)msg->buffer);
		}
		
		SysCloseHandle(msg->src);																									// Close the handles
		SysCloseHandle(msg->rport);
		MmFreeMemory((UIntPtr)msg);																									// And free the msg struct
	}
}

Boolean ServerInit(Void) {
	if (!IpcCreatePort(L"GuiServer")) {																								// First, create the GUI server port
		return False;																												// ...
	}
	
	return PsCreateThread((UIntPtr)GuiServerListener) != -1;																		// Create the listener thread
}
