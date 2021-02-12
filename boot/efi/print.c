/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 20 of 2021, at 11:15 BRT
 * Last edited on February 11 of 2021 at 12:03 BRT */

#include <efi/lib.h>

static Int32 EfiRound(float Value) {
    /* Dummy/unoptimized round function, if we add/subtract 0.5 from the value, and truncate, we gonna get the nearest
     * integer. */

    return (Int32)(Value < 0 ? Value - 0.5f : Value + 0.5f);
}

static UInt32 EfiBlendAlpha(UInt32 Background, UInt8 Red, UInt8 Green, UInt8 Blue, float Alpha) {
    UInt8 r, g, b;

    /* Same alpha blending that we use on the kernel include/img.hxx. */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    r = (UInt8)(Background >> 16);
    g = (UInt8)(Background >> 8);
    b = (UInt8)Background;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    r = (UInt8)(Background >> 8);
    g = (UInt8)(Background >> 16);
    b = (UInt8)(Background >> 24);
#else
#error Invalid byte order (expected little endian or big endian)
#endif

    Red = (UInt8)EfiRound((Alpha * Red) + ((1.f - Alpha) * r));
    Green = (UInt8)EfiRound((Alpha * Green) + ((1.f - Alpha) * g));
    Blue = (UInt8)EfiRound((Alpha * Blue) + ((1.f - Alpha) * b));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return 0xFF000000 | (Red << 16) | (Green << 8) | Blue;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return (Blue << 24) | (Green << 16) | (Red << 8) | 0xFF;
#else
#error Invalid byte order (expected little endian or big endian)
#endif
}

Void EfiDrawCharacter(Char8 Data, UInt16 X, UInt16 Y, UInt8 Red, UInt8 Green, UInt8 Blue) {
    if (X >= EfiGop->Mode->Info->Width || Y >= EfiGop->Mode->Info->Height) {
        return;
    }

    const EfiFontGlyph *info = &EfiFont.GlyphInfo[(UInt8)Data];
    const UInt8 *data = &EfiFont.GlyphData[info->Offset];
    UInt32 gx = info->Left, gy = EfiFont.Ascender - info->Top,
           *start = &((UInt32*)EfiGop->Mode->FrameBufferBase)[(Y + gy) * EfiGop->Mode->Info->PixelsPerScanLine +
                                                              X + gx];

    /* Using the same font drawing method as the kernel, for description of what exactly the code is doing, see
     * vid/image.cxx. */

    for (Int32 y = 0; y < info->Height; y++) {
        if (Y + gy + y >= EfiGop->Mode->Info->Height) {
            break;
        }
    
        for (Int32 x = 0; x < info->Width; x++) {
            if (X + gx + x >= EfiGop->Mode->Info->Width) {
                break;
            }

            UInt8 bright = data[y * info->Width + x];
            UInt32 *pos = &start[y * EfiGop->Mode->Info->PixelsPerScanLine + x];

            if (bright) {
                *pos = EfiBlendAlpha(*pos, Red, Green, Blue, (float)bright / 255);
            }
        }
    }
}

Void EfiGetStringMetrics(const Char8 *Data, UInt16 *Width, UInt16 *Height) {
    /* Each character of the font has it's own advance/width, but the entire font shares the same height (and it's all
     * ASCII, so we don't need to worry about vertical fonts), we still need to calculate the height, as there might
     * be NL (new line) characters in the text (for now, we're going to not consider line breaks from the text reaching
     * the end of the line).
     * TODO (Both for this and for the DrawString function): Add text formatting. */

    *Width = 0;
    *Height = EfiFont.Height;

    UInt16 x = 0;

    for (UIntN i = 0; Data[i]; i++) {
        switch (Data[i]) {
        case '\n': *Height += EfiFont.Height;
        case '\r': x = 0; break;
        default: x += EfiFont.GlyphInfo[(UInt8)Data[i]].Advance; break;
        }

        /* Thanks to the nl characters we need to make sure to only update the width if the current X is bigger than
         * the highest x. */

        if (x > *Width) {
            *Width = x;
        }
    }
}

Void EfiDrawString(const Char8 *Data, UInt16 X, UInt16 Y, UInt8 Red, UInt8 Green, UInt8 Blue) {
    /* Like EfiGetStringMetrics, but we don't need to save the width/height, and we need to also call DrawCharacter,
     * at least when it's not \n or \r. Also, we need to save the start/initial X, so that we correctly set the
     * X position to the start on new lines. */

    UInt16 sx = X;

    for (UIntN i = 0; Data[i]; i++) {
        switch (Data[i]) {
        case '\n': Y += EfiFont.Height;
        case '\r': X = sx; break;
        default: {
            EfiDrawCharacter(Data[i], X, Y, Red, Green, Blue);
            X += EfiFont.GlyphInfo[(UInt8)Data[i]].Advance;
            break;
        }
        }
    }
}
