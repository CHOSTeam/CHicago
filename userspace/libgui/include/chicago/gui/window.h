// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 22:45 BRT
// Last edited on December 26 of 2019, at 00:15 BRT

#ifndef __CHICAGO_GUI_WINDOW_H__
#define __CHICAGO_GUI_WINDOW_H__

#include <chicago/gui/img.h>

typedef struct {
	UIntPtr x;
	UIntPtr y;
	UIntPtr wkey;
	UIntPtr skey;
	PImage surface;
} GuiWindow, *PGuiWindow;

#ifdef __cplusplus
extern "C"
{
#endif

PGuiWindow GuiCreateWindow(UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_GUI_WINDOW_H__
