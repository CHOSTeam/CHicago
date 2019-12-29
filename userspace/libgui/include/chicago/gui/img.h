// File author is Ítalo Lima Marconato Matias
//
// Created on April 17 of 2019, at 21:57 BRT
// Last edited on December 25 of 2019, at 23:29 BRT

#ifndef __CHICAGO_GUI_IMG_H__
#define __CHICAGO_GUI_IMG_H__

#include <chicago/types.h>

#define BITBLIT_MODE_COPY 0x01
#define BITBLIT_MODE_BLEND 0x02
#define BITBLIT_MODE_INVERT 0x04

typedef struct {
	UInt8 b;
	UInt8 m;
	UInt32 sz;
	UInt16 res1;
	UInt16 res2;
	UInt32 off;
	UInt32 size;
	UInt32 width;
	UInt32 height;
	UInt16 planes;
	UInt16 bpp;
	UInt32 compression;
	UInt32 image_size;
	UInt32 xpels_per_meter;
	UInt32 ypels_per_meter;
	UInt32 clr_used;
	UInt32 clr_important;
} Packed BmpHeader, *PBmpHeader;

typedef struct {
	UIntPtr width;
	UIntPtr height;
	UInt8 bpp;
	UIntPtr buf;
} Image, *PImage;

#ifdef __cplusplus
extern "C"
{
#endif

PImage ImgCreate(UIntPtr width, UIntPtr height, UInt8 bpp);
PImage ImgCreateBuf(UIntPtr width, UIntPtr height, UInt8 bpp, UIntPtr buf);
PImage ImgLoadBMPBuf(PUInt8 buf);
PImage ImgLoadBMP(PWChar path);
PImage ImgRescale(PImage src, UIntPtr width, UIntPtr height);
Void ImgExtractRGB(UIntPtr c, PUInt8 r, PUInt8 g, PUInt8 b);
Void ImgExtractARGB(UIntPtr c, PUInt8 a, PUInt8 r, PUInt8 g, PUInt8 b);
UIntPtr ImgCreateRGB(UInt8 r, UInt8 g, UInt8 b);
UIntPtr ImgCreateARGB(UInt8 a, UInt8 r, UInt8 g, UInt8 b);
UIntPtr ImgInvertColorEndian(UIntPtr c);
Void ImgClear(PImage img, UIntPtr c);
Void ImgScroll(PImage img, UIntPtr c);
UIntPtr ImgGetPixel(PImage img, UIntPtr x, UIntPtr y);
Void ImgPutPixel(PImage img, UIntPtr x, UIntPtr y, UIntPtr c);
Void ImgDrawLine(PImage img, UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c);
Void ImgDrawRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void ImgFillRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c);
Void ImgDrawRoundedRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
Void ImgFillRoundedRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c);
UIntPtr ImgBlendColors(UIntPtr a, UIntPtr b);
Void ImgBitBlit(PImage img, PImage src, UIntPtr srcx, UIntPtr srcy, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UInt8 mode);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_GUI_IMG_H__
