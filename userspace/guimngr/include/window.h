// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 21:47 BRT
// Last edited on November 11 of 2019, at 15:14 BRT

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <img.h>

typedef struct {
	UIntPtr x;
	UIntPtr y;
	PImage surface;
} GuiWindow, *PGuiWindow;

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h);
Void GuiFreeWindow(PGuiWindow window);

#endif		// __WINDOW_H__
