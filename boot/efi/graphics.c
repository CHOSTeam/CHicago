/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 06 of 2021, at 15:31 BRT
 * Last edited on July 06 of 2021 at 20:08 BRT */

#include <efi/lib.h>

static inline UInt32 GetColor(UInt8 Red, UInt8 Green, UInt8 Blue) {
    /* Let's hope this is a newer version of clang/gcc, that implements the __BYTE_ORDER__ macro, else, we're going to
     * default the byte order as little endian (as the first #if will return true). */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return 0xFF000000 | (Red << 16) | (Green << 8) | Blue;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (Blue << 24) | (Green << 16) | (Red << 8) | 0xFF;
#else
#error Invalid byte order (expected little endian or big endian)
#endif
}

Void EfiFillScreen(UInt8 Red, UInt8 Green, UInt8 Blue) {
    /* Let's hope this is a newer version of clang/gcc, that implements the __BYTE_ORDER__ macro, else, we're going to
     * default the byte order as little endian (as the first #if will return true). */

    EfiSetMemory32((Void*)EfiGop->Mode->FrameBufferBase, EfiGop->Mode->Info->Width * EfiGop->Mode->Info->Height,
                   GetColor(Red, Green, Blue));
}

Void EfiPutPixel(UInt16 X, UInt16 Y, UInt8 Red, UInt8 Green, UInt8 Blue) {
    /* Same byte order check as the ClearScreen function, but we also need to truncate the X,Y pair if it is too big. */

    if (X >= EfiGop->Mode->Info->Width || Y >= EfiGop->Mode->Info->Height) return;

    /* No need to calculate pitch nor anything like that, as the size of each pixel will always be 32-bits here. */

    ((UInt32*)EfiGop->Mode->FrameBufferBase)[Y * EfiGop->Mode->Info->PixelsPerScanLine +
                                             X] = GetColor(Red, Green, Blue);
}

Void EfiDrawRectangle(UInt16 X, UInt16 Y, UInt16 Width, UInt16 Height, UInt8 Red, UInt8 Green, UInt8 Blue) {
    if (X >= EfiGop->Mode->Info->Width || Y >= EfiGop->Mode->Info->Height) return;

    /* Save the start, so that we don't have to recalculate it all the time. */

    UInt32 *start = &((UInt32*)EfiGop->Mode->FrameBufferBase)[Y * EfiGop->Mode->Info->PixelsPerScanLine + X],
           color = GetColor(Red, Green, Blue);
    UInt16 tw = X + Width >= EfiGop->Mode->Info->Width ? EfiGop->Mode->Info->Width - X : Width;

    for (UInt16 i = 0; i < Height; i++) {
        /* Now this is how it works: For the first and last lines, draw a whole line, for the other ones, just plot a
         * pixel on the start and on the end. */

        if (Y + i >= EfiGop->Mode->Info->Height) return;
        if (!i || i == Height - 1) EfiSetMemory32(start, tw, color);
        else {
            *start = color;
            if (tw == Width) start[Width - 1] = color;
        }

        /* And on the end, increase the "start" position. */

        start += EfiGop->Mode->Info->Width;
    }
}

Void EfiFillRectangle(UInt16 X, UInt16 Y, UInt16 Width, UInt16 Height, UInt8 Red, UInt8 Green, UInt8 Blue) {
    /* Very similar to DrawRectangle, but we just fill whole lines (that is, everytime is the i==0 case). */

    if (X >= EfiGop->Mode->Info->Width || Y >= EfiGop->Mode->Info->Height) return;

    UInt32 *start = &((UInt32*)EfiGop->Mode->FrameBufferBase)[Y * EfiGop->Mode->Info->PixelsPerScanLine + X],
           color = GetColor(Red, Green, Blue);
    UInt16 tw = X + Width >= EfiGop->Mode->Info->Width ? EfiGop->Mode->Info->Width - X : Width;

    /* Alternative way to fill the screen: If the X is 0 and the width is the screen's width, we can SetMemory the whole
     * region, while skipping the first Y lines. */
    
    if (!X && Width == EfiGop->Mode->Info->Width) {
        EfiSetMemory32(start, Height * EfiGop->Mode->Info->Width, color);
        return;
    }

    for (UInt16 i = 0; i < Height; i++) {
        if (Y + i >= EfiGop->Mode->Info->Height) return;
        EfiSetMemory32(&start[i * EfiGop->Mode->Info->Width], tw, color);
    }
}

EfiStatus EfiSetupGraphics(UInt8 Red, UInt8 Green, UInt8 Blue) {
    EfiStatus status;
    UIntN size = 0, max = 0, maxi = 0;
    EfiGraphicsOutputModeInformation *mode;

    for (UIntN i = 0; i < EfiGop->Mode->MaxMode; i++) {
        /* Only the pixel format needs to be an exact match. */

        if (!EFI_ERROR((status = EfiGop->QueryMode(EfiGop, i, &size, &mode))) && mode->Width <= 1920 &&
            mode->Height <= 1080 && mode->Width * mode->Height > max && mode->PixelFormat == EfiPixelFormatBGRR8)
            max = mode->Width * mode->Height, maxi = i;
    }

    if (!max) return EFI_UNSUPPORTED;
    else if (EFI_ERROR((status = EfiGop->SetMode(EfiGop, maxi)))) return status;

    EfiFillScreen(Red, Green, Blue);

    return EFI_SUCCESS;
}
