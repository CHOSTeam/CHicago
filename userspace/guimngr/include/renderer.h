// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 21:41 BRT
// Last edited on December 25 of 2019, at 20:37 BRT

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <window.h>

Boolean RendererLoadTheme(PWChar path);
PGuiWindow GuiGetWindow(UIntPtr key);
Void GuiAddWindow(PGuiWindow window);
Void GuiRemoveWindow(PGuiWindow window);
Void GuiRefresh(Void);
Boolean RendererInit(Void);

#endif		// __RENDERER_H__
