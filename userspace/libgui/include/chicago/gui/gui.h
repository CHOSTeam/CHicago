// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 23:23 BRT
// Last edited on December 29 of 2019, at 18:09 BRT

#ifndef __CHICAGO_GUI_GUI_H__
#define __CHICAGO_GUI_GUI_H__

#include <chicago/gui/window.h>

#define GUI_CREATE_WINDOW_REQUEST 0x00
#define GUI_REMOVE_WINDOW_REQUEST 0x01
#define GUI_REFRESH_REQUEST 0x02

typedef struct {
	UIntPtr x;
	UIntPtr y;
	UIntPtr w;
	UIntPtr h;
} GuiCreateWindowRequest, *PGuiCreateWindowRequest;

typedef struct {
	UIntPtr shm_key;
	UIntPtr window_key;
	UIntPtr bpp;
} GuiCreateWindowReply, *PGuiCreateWindowReply;

#ifdef __cplusplus
extern "C"
{
#endif

Void GuiInit(Void);
Void GuiRefresh(Void);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_GUI_GUI_H__
