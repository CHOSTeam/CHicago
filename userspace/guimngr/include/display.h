// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2019, at 16:03 BRT
// Last edited on November 10 of 2019, at 16:16 BRT

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <img.h>

#ifndef __DISPLAY__
extern PImage DispBackBuffer;
#endif

Void DispRefresh(Void);
Boolean DispInit(Void);

#endif		// __DISPLAY_H__
