// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on December 25 of 2019, at 19:28 BRT

#include <chicago/process.h>

#include <display.h>
#include <renderer.h>

Void AppEntry(Void) {
	if (!DispInit()) {															// Get the display info and init the backbuffer
		PsExitProcess((UIntPtr)-1);
	} else if (!RendererInit()) {												// Init the renderer
		PsExitProcess((UIntPtr)-1);
	}
	
	while (True) ;
}
