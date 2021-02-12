/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 21 of 2021, at 14:55 BRT
 * Last edited on February 11 of 2021 at 12:01 BRT */

#include <arch.h>
#include <config.h>
#include <efi/lib.h>
#include <loader.h>

static const Char8 *MenuName = "CHicago Boot Manager", *MenuVersion = "Version next-4";
static UIntN MenuEntryMax = 0, MenuEntryCount = 0;
static MenuEntry **MenuEntries = Null;

static Void MenuDrawDecoration(UIntN Reserved, UIntN NameWidth, UIntN VerWidth) {
    /* Putting the code to draw the menu decorations/top and bottom bars in a separated function, just to prevent some
     * repetition. */
    
    EfiFillRectangle(0, 0, EfiGop->Mode->Info->Width, Reserved, 0x45, 0x98, 0xAA);
    EfiFillRectangle(0, EfiGop->Mode->Info->Height - Reserved, EfiGop->Mode->Info->Width, Reserved, 0x45, 0x98, 0xAA);
    EfiDrawString(MenuName, (EfiGop->Mode->Info->Width - NameWidth) / 2, 5, 0x00, 0x00, 0x00);
    EfiDrawString(MenuVersion, (EfiGop->Mode->Info->Width - VerWidth) / 2, EfiGop->Mode->Info->Height - Reserved + 5,
                  0x00, 0x00, 0x00);
}

static MenuEntry *MenuAddEntry(const Char8 *Name, const Char16 *Path, MenuEntryType Type) {
    /* Adding a new entry is easy: Allocate the entry struct (and fill it), expand the entry array (if required), save
     * entry into the right array index, increase the entry count, and we're done!
     * TODO: Maybe return a status code? It could be useful to at least inform the user when some entry fails to load. */

    MenuEntry *entry = EfiAllocatePool(sizeof(MenuEntry)), **new;
    UIntN cur = MenuEntryMax * sizeof(MenuEntry*), newc = cur ? cur * 2 : sizeof(MenuEntry*);

    if (entry == Null) {
        return Null;
    } else if ((entry->Name = EfiDuplicateString8(Name)) == Null) {
        EfiFreePool(entry);
        return Null;
    } else if ((entry->Path = EfiDuplicateString16(Path)) == Null) {
        EfiFreePool(entry->Name);
        EfiFreePool(entry);
        return Null;
    } else if (MenuEntryMax - MenuEntryCount <= 0 && (new = EfiReallocatePool(MenuEntries, cur, newc)) == Null) {
        EfiFreePool(entry->Path);
        EfiFreePool(entry->Name);
        EfiFreePool(entry);
        return Null;
    } else if (MenuEntryMax - MenuEntryCount <= 0) {
        MenuEntries = new;
        MenuEntryMax = newc / sizeof(MenuEntry*);
    }

    entry->Type = Type;
    MenuEntries[MenuEntryCount++] = entry;

    return entry;
}

Void MenuAddCHicagoEntry(const Char8 *Name, const Char16 *Path, UIntN ImageIndex) {
    MenuEntry *entry = MenuAddEntry(Name, Path, MenuEntryCHicago);

    if (entry != Null) {
        entry->CHicago.ImageIndex = ImageIndex;
    }
}

Void MenuStart(Void) {
    /* Before drawing the top bar (that goes with the name/version), let's get the size of the menu and version strings,
     * so that we can properly center it all. */

    UInt16 namew, verw, disch, res = EfiFont.Height + 10;
    EfiGetStringMetrics(MenuName, &namew, &disch);
    EfiGetStringMetrics(MenuVersion, &verw, &disch);

    /* Parse the config file and check if we have any boot entry. */

    if (EFI_ERROR(CfgParse())) {
        MenuDrawDecoration(res, namew, verw);
        return;
    } else if (!MenuEntryCount) {
        MenuDrawDecoration(res, namew, verw);
        EfiDrawString("The configuration file has no boot entries.", 5, res + 5, 0xFF, 0xFF, 0xFF);
        return;
    }

    IntN sel = 0, start = 0, max = 0;

s:  /* Draw the top/bottom bars and announce the boot manager name and version. */

    EfiFillScreen(0x00, 0x00, 0x0);
    MenuDrawDecoration(res, namew, verw);

    for (UIntN i = start; i < MenuEntryCount; i++) {
        if (res + (EfiFont.Height + 15) * (i - start) + 10 > EfiGop->Mode->Info->Height - res) {
            max = i;
            break;
        }

        EfiDrawString(MenuEntries[i]->Name, 5, res + (EfiFont.Height + 15) * (i - start) + 10, 0xFF, 0xFF, 0xFF);
    }

    /* Now wait for any key, and if it is the up/down arrow, manipulate the 'sel' variable (making sure that it
     * also doesn't under/overflow). */

    EfiDrawRectangle(0, res + (EfiFont.Height + 15) * (sel - start) + 5, EfiGop->Mode->Info->Width, res,
                     0xFF, 0xFF, 0xFF);

    while (True) {
        EfiEvent event = EfiConIn->WaitForKey;
        EfiInputKey key;
        IntN old = sel;
        UIntN index;
    
        EfiBS->WaitForEvent(1, &event, &index);
        EfiConIn->ReadKeyStroke(EfiConIn, &key);

        /* Maybe we should use macros (or an enum) for the scan codes, instead of hardcoding? */

        if (key.ScanCode == 0x01 && sel > 0) {
            sel--;
        } else if (key.ScanCode == 0x02 && (UIntN)(sel + 1) < MenuEntryCount) {
            sel++;
        } else if (key.Unicode == '\r') {
            /* Enter means that we should boot this entry... LET'S GO! */
        
            MenuEntry *entry = MenuEntries[sel];

            EfiFillRectangle(0, res, EfiGop->Mode->Info->Width, EfiGop->Mode->Info->Height - (res * 2),
                             0x00, 0x00, 0x00);

            switch(entry->Type) {
            case MenuEntryCHicago: LdrStartCHicago(entry); break;
            default: {
                EfiDrawString("The entry type is invalid.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
                break;
            }
            }

            return;
        }

        /* For scrolling, we can check if the (newly) selected entry is inside of the visible area, if it isn't,
         * determine if it is above or below it (above means that we need to decrease our start variable, and below
         * means that we need to increase it). */

        if (sel != old && sel - start < 0) {
            start--;
            max = 0;
            goto s;
        } else if (sel != old && max && sel >= max) {
            start++;
            max = 0;
            goto s;
        } else if (sel != old) {
            EfiDrawRectangle(0, res + (EfiFont.Height + 15) * (old - start) + 5, EfiGop->Mode->Info->Width, res,
                             0x00, 0x00, 0x00);
            EfiDrawRectangle(0, res + (EfiFont.Height + 15) * (sel - start)  + 5, EfiGop->Mode->Info->Width, res,
                             0xFF, 0xFF, 0xFF);
        }
    }
}
