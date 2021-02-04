/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 21 of 2021, at 14:53 BRT
 * Last edited on January 28 of 2021 at 15:14 BRT */

#pragma once

#include <efi/types.h>

typedef enum {
    MenuEntryCHicago
} MenuEntryType;

typedef struct {
    Char8 *Name;
    Char16 *Path;
    MenuEntryType Type;
    union {
        struct {
            UIntN ImageIndex;
        } CHicago;
    };
} MenuEntry;

Void MenuAddCHicagoEntry(const Char8*, const Char16*, UIntN);
Void MenuStart(Void);
