// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on November 11 of 2019, at 15:44 BRT

#include <chicago/process.h>

#include <display.h>
#include <renderer.h>

Void AppEntry(Void) {
	if (!DispInit()) {												// Get the display info and init the backbuffer
		PsExitProcess((UIntPtr)-1);
	} else if (!RendererInit()) {									// Init the renderer
		PsExitProcess((UIntPtr)-1);
	}
	
	GuiAddWindow(GuiCreateWindow(200, 300, 300, 200));				// Create some test windows
	GuiAddWindow(GuiCreateWindow(400, 400, 400, 250));
	GuiAddWindow(GuiCreateWindow(600, 20, 600, 600));
	
	while (True) ;
}
