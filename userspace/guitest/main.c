// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 22:09 BRT
// Last edited on December 29 of 2019, at 18:18 BRT

#include <chicago/gui/gui.h>

Void AppEntry(Void) {
	GuiInit();																// Init everything
	
	PGuiWindow w1 = GuiCreateWindow(50, 50, 400, 400);						// Create some test windows
	PGuiWindow w2 = GuiCreateWindow(100, 100, 200, 200);
	PGuiWindow w3 = GuiCreateWindow(0, 0, 100, 100);
	
	while (True) ;
}
