// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 20:03 BRT
// Last edited on December 26 of 2019, at 00:19 BRT

#ifndef __SERVER_H__
#define __SERVER_H__

#include <chicago/types.h>

#define GUI_CREATE_WINDOW_REQUEST 0x00

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

Boolean ServerInit(Void);

#endif		// __SERVER_H__
