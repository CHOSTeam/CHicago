// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 21:47 BRT
// Last edited on December 25 of 2019, at 20:54 BRT

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <img.h>

typedef struct {
	UIntPtr x;
	UIntPtr y;
	UIntPtr key;
	PImage surface;
} GuiWindow, *PGuiWindow;

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h);
PGuiWindow GuiCreateWindowBuf(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr buf);
Void GuiFreeWindow(PGuiWindow window);

#endif		// __WINDOW_H__
