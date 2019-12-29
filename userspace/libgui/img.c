// File author is Ítalo Lima Marconato Matias
//
// Created on April 18 of 2019, at 12:04 BRT
// Last edited on December 25 of 2019, at 23:30 BRT

#include <chicago/alloc.h>
#include <chicago/file.h>
#include <chicago/misc.h>
#include <chicago/string.h>
#include <chicago/gui/img.h>

PImage ImgCreate(UIntPtr width, UIntPtr height, UInt8 bpp) {
	if ((width == 0) || (height == 0) || ((bpp != 3) && (bpp != 4))) {															// Sanity checks
		return Null;
	}
	
	PImage img = (PImage)MmAllocMemory(sizeof(Image) + width * height * bpp);													// Alloc the img struct + the buffer
	
	if (img != Null) {																											// Failed?
		img->width = width;																										// Nope, so let's setup everything
		img->height = height;
		img->bpp = bpp;
		img->buf = ((UIntPtr)img) + sizeof(Image);
	}
	
	return img;
}

PImage ImgCreateBuf(UIntPtr width, UIntPtr height, UInt8 bpp, UIntPtr buf) {
	if ((width == 0) || (height == 0) || ((bpp != 3) && (bpp != 4))) {															// Sanity checks
		return Null;
	}
	
	PImage img = (PImage)MmAllocMemory(sizeof(Image));																			// Alloc the img struct
	
	if (img != Null) {																											// Failed?
		img->width = width;																										// Nope, so let's setup everything
		img->height = height;
		img->bpp = bpp;
		img->buf = buf;
	}
	
	return img;
}

PImage ImgLoadBMPBuf(PUInt8 buf) {
	if (buf == Null) {																											// Sanity check
		return Null;
	}
	
	PBmpHeader hdr = (PBmpHeader)buf;																							// Get the BMP file header
	
	if ((hdr->b != 'B') || (hdr->m != 'M')) {																					// Check if this is really a BMP file
		return Null;
	} else if ((hdr->planes != 1) || ((hdr->bpp != 24) && (hdr->bpp != 32)) || (hdr->compression != 0)) {						// Check if we support this bmp file (we only support 24 and 32bpp, and we don't support compression)
		return Null;
	}
	
	PImage img = ImgCreate(hdr->width, hdr->height, hdr->bpp / 8);																// Create the return img struct
	
	if (img == Null) {
		return Null;																											// Failed :(
	}
	
	UIntPtr bmp = ((UIntPtr)buf) + hdr->off;																					// Let's copy the bitmap
	UIntPtr pad = (((hdr->width * img->bpp) + 3) & -4) - (hdr->width * img->bpp);												// Calculate how many padding bytes we have
	
	for (UIntPtr i = hdr->height - 1, j = 0; j < hdr->height; i--, j++) {
		StrCopyMemory((PUInt8)(img->buf + i * hdr->width * img->bpp), (PUInt8)bmp, hdr->width * img->bpp);						// Copy the scanline
		bmp += hdr->width * img->bpp + pad;																						// And go to the next scanline
	}
	
	return img;
}

PImage ImgLoadBMP(PWChar path) {
	if (path == Null) {																											// Sanity check
		return Null;
	}
	
	IntPtr file = FsOpenFile(path);																								// Let's try to open the file
	
	if (file == -1) {
		return Null;																											// Failed...
	}
	
	UIntPtr len = FsGetFileSize(file);																							// Get the file length
	PUInt8 buf = (PUInt8)MmAllocMemory(len);																					// And alloc memory for reading it
	
	if (buf == Null) {
		SysCloseHandle(file);																									// Failed, close everything
		return Null;
	} else if (!FsReadFile(file, len, buf)) {																					// Read the file into memory
		MmFreeMemory((UIntPtr)buf);																								// Failed, free and close everything
		SysCloseHandle(file);
		return Null;
	}
	
	PImage ret = ImgLoadBMPBuf(buf);																							// Load the image into memory
	
	MmFreeMemory((UIntPtr)buf);																									// Free the buffer
	SysCloseHandle(file);																										// Close the file
	
	return ret;																													// And return!
}

PImage ImgRescale(PImage src, UIntPtr width, UIntPtr height) {
	if (src == Null || (width == 0 && height == 0)) {																			// First, sanity check
		return Null;
	}
	
	PImage img = ImgCreate(width, height, src->bpp);																			// Create the new image
	
	if (img == Null) {
		return Null;																											// ...
	}
	
	Float sw = (Float)width / (Float)src->width;																				// Get all the scaled values
	Float sh = (Float)height / (Float)src->height;
	
	for (UIntPtr y = 0; y < height; y++) {																						// And let's do it!
		for (UIntPtr x = 0; x < width; x++) {
			ImgPutPixel(img, x, y, ImgGetPixel(src, (UIntPtr)(x / sw), (UIntPtr)(y / sh)));
		}
	}
	
	return img;
}

Void ImgExtractRGB(UIntPtr c, PUInt8 r, PUInt8 g, PUInt8 b) {
	if (r != Null) {																											// Extract the R (Red)
		*r = (UInt8)((c >> 16) & 0xFF);
	}
	
	if (g != Null) {																											// Extract the G (Green)
		*g = (UInt8)((c >> 8) & 0xFF);
	}
	
	if (b != Null) {																											// Extract the B (Blue)
		*b = (UInt8)(c & 0xFF);
	}
}

Void ImgExtractARGB(UIntPtr c, PUInt8 a, PUInt8 r, PUInt8 g, PUInt8 b) {
	if (a != Null) {																											// Extract the A (Alpha)
		*a = (UInt8)((c >> 24) & 0xFF);
	}
	
	if (r != Null) {																											// Extract the R (Red)
		*r = (UInt8)((c >> 16) & 0xFF);
	}
	
	if (g != Null) {																											// Extract the G (Green)
		*g = (UInt8)((c >> 8) & 0xFF);
	}
	
	if (b != Null) {																											// Extract the B (Blue)
		*b = (UInt8)(c & 0xFF);
	}
}

UIntPtr ImgCreateRGB(UInt8 r, UInt8 g, UInt8 b) {
	return (r << 16) | (g << 8) | b;
}

UIntPtr ImgCreateARGB(UInt8 a, UInt8 r, UInt8 g, UInt8 b) {
	return (a << 24) | (r << 16) | (g << 8) | b;
}

UIntPtr ImgInvertColorEndian(UIntPtr c) {
	UInt8 a;																													// First, let's extract the A, R, G and B fields
	UInt8 r;
	UInt8 g;
	UInt8 b;
	
	ImgExtractARGB(c, &a, &r, &g, &b);
	
	return ImgCreateARGB(b, g, r, a);																							// Now, use the ImgCreateARGB function to invert it
}

Void ImgClear(PImage img, UIntPtr c) {
	if (img == Null) {																											// Sanity check
		return;
	}
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	c = ImgInvertColorEndian(c);																								// Big endian...
#endif
	
	if (img->bpp == 3) {																										// Fill the image with the color c
		StrSetMemory24((PUInt32)img->buf, c, img->width * img->height);
	} else if (img->bpp == 4) {
		StrSetMemory32((PUInt32)img->buf, c, img->width * img->height);
	}
}

Void ImgScroll(PImage img, UIntPtr c) {
	if (img == Null) {																											// Sanity check
		return;
	}
	
	UIntPtr lsz = img->width * 16;
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	c = ImgInvertColorEndian(c);																								// Big endian...
#endif
	
	if (img->bpp == 3) {																										// Scroll the screen contents up and clear the last line!
		StrCopyMemory24((PUInt32)img->buf, (PUInt32)(img->buf + (lsz * 3)), img->width * img->height - lsz);
		StrSetMemory24((PUInt32)(img->buf + (img->width * img->height * 3) - (lsz * 3)), c, lsz);
	} else if (img->bpp == 4) {
		StrCopyMemory32((PUInt32)img->buf, (PUInt32)(img->buf + (lsz * 4)), img->width * img->height - lsz);
		StrSetMemory32((PUInt32)(img->buf + (img->width * img->height * 4) - (lsz * 4)), c, lsz);
	}
}

UIntPtr ImgGetPixel(PImage img, UIntPtr x, UIntPtr y) {
	if (img == Null) {																											// Sanity check
		return 0;
	} else if (x >= img->width) {																								// Fix the x and the y if they are bigger than the image dimensions	
		x = img->width - 1;
	}
	
	if (y >= img->height) {
		y = img->height - 1;
	}
	
	UIntPtr c = 0xFF000000;
	
	if (img->bpp == 3) {																										// Get the pixel!
		UInt16 lp = *((PUInt16)(img->buf + (y * (img->width * 3)) + (x * 3)));
		UInt8 hp = *((PUInt8)(img->buf + (y * (img->width * 3)) + (x * 3) + 2));
		c |= lp | (hp << 16);
	} else if (img->bpp == 4) {
		c = *((PUInt32)(img->buf + (y * (img->width * 4)) + (x * 4)));
	}
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	c = ImgInvertColorEndian(c);																								// Big endian...
#endif
	
	return c;
}

Void ImgPutPixel(PImage img, UIntPtr x, UIntPtr y, UIntPtr c) {
	if (img == Null) {																											// Sanity check
		return;
	} else if (x >= img->width) {																								// Fix the x and the y if they are bigger than the image dimensions	
		x = img->width - 1;
	}
	
	if (y >= img->height) {
		y = img->height - 1;
	}
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	c = ImgInvertColorEndian(c);																								// Big endian...
#endif
	
	if (img->bpp == 3) {																										// Write the pixel!
		*((PUInt16)(img->buf + (y * (img->width * 3)) + (x * 3))) = (UInt16)c;
		*((PUInt8)(img->buf + (y * (img->width * 3)) + (x * 3) + 2)) = (UInt8)(c >> 16);
	} else if (img->bpp == 4) {
		*((PUInt32)(img->buf + (y * (img->width * 4)) + (x * 4))) = (UInt32)c;
	}
}

Void ImgDrawLine(PImage img, UIntPtr x0, UIntPtr y0, UIntPtr x1, UIntPtr y1, UIntPtr c) {
	if (img == Null) {																											// Sanity check
		return;
	} else if (x0 >= img->width) {																								// Fix the x and the y if they are bigger than the image dimensions
		x0 = img->width - 1;																									//     Sorry, this function will not have a lot of documentation (not that my other ones are commented in a good way...)
	}
	
	if (y0 >= img->height) {
		y0 = img->height - 1;
	}
	
	if (x1 >= img->width) {
		x1 = img->width - 1;
	}
	
	if (y1 >= img->height) {
		y1 = img->height - 1;
	}
	
	IntPtr dx = x1 - x0;																										// Let's get the line direction
	IntPtr dy = y1 - y0;
	IntPtr sdx = (dx < 0) ? -1 : 1;
	IntPtr sdy = (dy < 0) ? -1 : 1;
	IntPtr px = x0;
	IntPtr py = y0;
	IntPtr x = 0;
	IntPtr y = 0;
	
	dx = sdx * dx + 1;
	dy = sdy * dy + 1;
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	c = ImgInvertColorEndian(c);																								// Big endian...
#endif
	
	if (dx >= dy) {																												// This line is more horizontal?
		for (x = 0; x < dx; x++) {																								// Yes
			if (img->bpp == 3) {
				*((PUInt16)(img->buf + (py * (img->width * 3)) + (px * 3))) = (UInt16)c;
				*((PUInt8)(img->buf + (py * (img->width * 3)) + (px * 3) + 2)) = (UInt8)(c << 16);
			} else if (img->bpp == 4) {
				*((PUInt32)(img->buf + (py * (img->width * 4)) + (px * 4))) = (UInt32)c;
			}
			
			y += dy;
			
			if (y >= dx) {
				y -= dx;
				py += sdy;
			}
			
			px += sdx;
		}
	} else {
		for (y = 0; y < dy; y++) {																								// No, so is more vertical
			if (img->bpp == 3) {
				*((PUInt16)(img->buf + (py * (img->width * 3)) + (px * 3))) = (UInt16)c;
				*((PUInt8)(img->buf + (py * (img->width * 3)) + (px * 3) + 2)) = (UInt8)(c << 16);
			} else if (img->bpp == 4) {
				*((PUInt32)(img->buf + (py * (img->width * 4)) + (px * 4))) = (UInt32)c;
			}
			
			x += dx;
			
			if (x >= dy) {
				x -= dy;
				px += sdx;
			}
			
			py += sdy;
		}
	}
}

Void ImgDrawRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	ImgDrawLine(img, x, y, x, y + h - 1, c);																					// Let's use the ImgDrawLine to draw the rectangle (we just need to draw 4 lines)
	ImgDrawLine(img, x, y, x + w - 1, y, c);
	ImgDrawLine(img, x, y + h - 1, x + w - 1, y + h - 1, c);
	ImgDrawLine(img, x + w - 1, y, x + w - 1, y + h - 1, c);
}

Void ImgFillRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr c) {
	if (img == Null) {																											// Sanity check
		return;
	} else if (x >= img->width) {																								// Fix the x and the y if they are bigger than the image dimensions	
		x = img->width - 1;
	}
	
	if (y >= img->height) {
		y = img->height - 1;
	}
	
	if ((x + w) > img->width) {																									// Fix the width and height of our rectangle if they are bigger than the screen dimensions
		w = img->width - x;
	}
	
	if ((y + h) > img->height) {
		h = img->height - y;
	}
	
	PUInt32 fb = (PUInt32)(img->buf + (y * (img->width * img->bpp)) + (x * img->bpp));
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	c = ImgInvertColorEndian(c);																								// Big endian...
#endif
	
	for (UIntPtr i = 0; i < h; i++) {																							// And fill our rectangle
		if (img->bpp == 3) {
			StrSetMemory24(fb, c, w);
		} else if (img->bpp == 4) {
			StrSetMemory32(fb, c, w);
		}
		
		fb = (PUInt32)(((UIntPtr)fb) + img->width * img->bpp);
	}
}

Void ImgDrawRoundedRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c) {
	if (r == 0) {																												// Radius is 0?
		ImgDrawRectangle(img, x, y, w, h, c);																					// Yes, so just draw an rectangle
		return;
	}
	
	IntPtr f = 1 - r;
	IntPtr dfx = 1;
	IntPtr dfy = -2 * r;
	UIntPtr xx = 0;
	UIntPtr yy = r;
	
	if (w > 0) {
		w--;
	}
	
	if (h > 0) {
		h--;
	}
	
	while (xx < yy) {
		if (f >= 0) {
			yy--;
			dfy += 2;
			f += dfy;
		}
		
		xx++;
		dfx += 2;
		f += dfx;
		
		ImgPutPixel(img, x - xx + r, y + h + yy - r, c);																		// Bottom left
		ImgPutPixel(img, x - yy + r, y + h + xx - r, c);
		ImgPutPixel(img, x - xx + r, y - yy + r, c);																			// Top left
		ImgPutPixel(img, x - yy + r, y - xx + r, c);
		ImgPutPixel(img, x + w + xx - r, y + h + yy - r, c);																	// Bottom right
		ImgPutPixel(img, x + w + yy - r, y + h + xx - r, c);
		ImgPutPixel(img, x + w + xx - r, y - yy + r, c);																		// Top right
		ImgPutPixel(img, x + w + yy - r, y - xx + r, c);
	}
	
	ImgDrawLine(img, x + r, y, x + w - r, y, c);																				// Draw the top line
	ImgDrawLine(img, x + r, y + h, x + w - r, y + h, c);																		// The bottom one
	ImgDrawLine(img, x, y + r, x, y + h - r, c);																				// The left one
	ImgDrawLine(img, x + w, y + r, x + w, y + h - r, c);																		// And, the right one!
}

Void ImgFillRoundedRectangle(PImage img, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UIntPtr r, UIntPtr c) {
	if (r == 0) {																												// Radius is 0?
		ImgFillRectangle(img, x, y, w, h, c);																					// Yes, so just fill an rectangle
		return;
	}
	
	IntPtr f = 1 - r;
	IntPtr dfx = 1;
	IntPtr dfy = -2 * r;
	UIntPtr xx = 0;
	UIntPtr yy = r;
	
	if (w > 0) {
		w--;
	}
	
	if (h > 0) {
		h--;
	}
	
	while (xx < yy) {
		if (f >= 0) {
			yy--;
			dfy += 2;
			f += dfy;
		}
		
		xx++;
		dfx += 2;
		f += dfx;
		
		ImgDrawLine(img, x - xx + r, y + h + yy - r, x - xx + r, y - yy + r, c);												// Left
		ImgDrawLine(img, x - yy + r, y + h + xx - r, x - yy + r, y - xx + r, c);
		ImgDrawLine(img, x + w + xx - r, y + h + yy - r, x + w + xx - r, y - yy + r, c);										// Right
		ImgDrawLine(img, x + w + yy - r, y + h + xx - r, x + w + yy - r, y - xx + r, c);
	}
	
	ImgFillRectangle(img, x + r, y, w - r * 2 + 1, h + 1, c);																	// Fill the center rectangle
	ImgDrawLine(img, x, y + r, x, y + h - r, c);																				// The left line
	ImgDrawLine(img, x + w, y + r, x + w, y + h - r, c);																		// And the right one
}

UIntPtr ImgBlendColors(UIntPtr a, UIntPtr b) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	a = ImgInvertColorEndian(a);																								// Big endian...
	b = ImgInvertColorEndian(b);
#endif
	
	UIntPtr ba = (b >> 24) & 0xFF;																								// Extract the alpha value from the B val
	
	if (ba == 0) {																												// Transparent?
		return a;																												// Yes, just return the A val
	}
	
	UIntPtr rb = (((b & 0xFF00FF) * ba) + ((a & 0xFF00FF) * (0xFF - ba))) & 0xFF00FF00;											// Extract and blend the red and blue (0xFF00FF)
	UIntPtr g = (((b & 0xFF00) * ba) + ((a & 0xFF00) * (0xFF - ba))) & 0xFF0000;												// Extract and blend the green (0xFF0000)
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return ImgInvertColorEndian((b & 0xFF000000) | ((rb | g) >> 8));															// Big endian, we need to invert the blended color
#else
	return (b & 0xFF000000) | ((rb | g) >> 8);																					// Little endian, just return the blended color
#endif
}

Void ImgBitBlit(PImage img, PImage src, UIntPtr srcx, UIntPtr srcy, UIntPtr x, UIntPtr y, UIntPtr w, UIntPtr h, UInt8 mode) {
	if (img == Null || src == Null) {																							// Sanity check
		return;
	} else if (((mode & BITBLIT_MODE_COPY) != BITBLIT_MODE_COPY) && ((mode & BITBLIT_MODE_BLEND) != BITBLIT_MODE_BLEND)) {		// Invalid mode?
		return;																													// Yes :(
	} else if (srcx >= src->width) {																							// Fix the srcx and the srcy if they are bigger than the image dimensions
		srcx = src->width - 1;
	}
	
	if (srcy >= src->height) {
		srcy = src->height - 1;
	}
	
	if ((srcx + w) > src->width) {																								// Fix the width and height if they are bigger than the image dimensions
		w = src->width - srcx;
	}
	
	if ((srcy + h) > src->height) {
		h = src->height - srcy;
	}
	
	if (x >= img->width) {																										// Fix the x and the y if they are bigger than the image dimensions
		x = img->width - 1;
	}
	
	if (y >= img->height) {
		y = img->height - 1;
	}
	
	if ((x + w) > img->width) {																									// Fix the width and height if they are bigger than the image dimensions
		w = img->width - x;
	}
	
	if ((y + h) > img->height) {
		h = img->height - y;
	}
	
	PUInt8 srcb = (PUInt8)(src->buf + (srcy * src->width * src->bpp) + (srcx * src->bpp));
	PUInt8 dstb = (PUInt8)(img->buf + (y * img->width * img->bpp) + (x * img->bpp));
	
	if ((src->bpp == img->bpp) && ((mode & BITBLIT_MODE_COPY) == BITBLIT_MODE_COPY)) {											// Same BPP and mode is BITBLIT_MODE_COPY?
		Boolean invert = (mode & BITBLIT_MODE_INVERT) == BITBLIT_MODE_INVERT;													// Yes, first let's see if we need to copy the lines in the reverse order
		
		for (UIntPtr ys = invert ? h - 1 : 0, yd = 0; yd < h; invert ? ys-- : ys++, yd++) {										// Now, let's copy them!			
			StrCopyMemory(&dstb[yd * img->width * src->bpp], &srcb[ys * src->width * src->bpp], w * src->bpp);
		}
	} else if ((mode & BITBLIT_MODE_COPY) == BITBLIT_MODE_COPY) {																// mode is BITBLIT_MODE_COPY?
		Boolean invert = (mode & BITBLIT_MODE_INVERT) == BITBLIT_MODE_INVERT;													// Yes, first let's see if we need to copy the lines in the reverse order
		
		for (UIntPtr ys = invert ? h + 1 : 0, yd = 0; invert ? ys != 0 : ys < h; invert ? ys-- : ys++, yd++) {					// Now, let's copy them!
			PUInt8 row = &srcb[ys * src->width * src->bpp];
			
			for (UIntPtr xx = 0; xx < w; xx++) {
				if (src->bpp == 3) {
					ImgPutPixel(img, x + xx, y + yd, ((row[xx * 3 + 2] << 16) | (row[xx * 3 + 1] << 8) | row[xx * 3]) | 0xFF000000);
				} else if (src->bpp == 4) {
					ImgPutPixel(img, x + xx, y + yd, *((PUInt32)(row + xx * 4)));
				}
			}
		}
	} else {																													// mode should be DISP_MODE_BLEND
		Boolean invert = (mode & BITBLIT_MODE_INVERT) == BITBLIT_MODE_INVERT;													// Yes, first let's see if we need to copy the lines in the reverse order
		
		for (UIntPtr ys = invert ? src->height + 1 : 0, yd = 0; invert ? ys != 0 : ys < h; invert ? ys-- : ys++, yd++) {		// Now, let's copy them!
			PUInt8 row = &srcb[ys * src->width * src->bpp];
			
			for (UIntPtr xx = 0; xx < w; xx++) {
				UIntPtr a = ImgGetPixel(img, x + xx, y + yd);																	// Get the A pixel
				UIntPtr b = 0;																									// Get the B pixel
				
				if (src->bpp == 3) {
					b = ((row[xx * 3 + 2] << 16) | (row[xx * 3 + 1] << 8) | row[xx * 3]) | 0xFF000000;
				} else if (src->bpp == 4) {
					b = *((PUInt32)(row + xx * 4));
				}
				
				ImgPutPixel(img, x + xx, y + yd, ImgBlendColors(a, b));															// Blend the colors and put the pixel
			}
		}
	}
}
