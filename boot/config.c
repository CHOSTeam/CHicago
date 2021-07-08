/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 24 of 2021, at 10:27 BRT
 * Last edited on July 04 of 2021 at 11:08 BRT */

#include <config.h>
#include <efi/lib.h>
#include <menu.h>

static Void CfgConsumeWhiteSpaces(const Char8 *Text, UIntN *Pos, Boolean NewLines) {
    /* Whitespace characters: Space, Tab (\t), Vertical Tab (\v), Carriage Return (\r), Line Feed (\n) and Form Feed
     * (\f). Btw, the last three we should only consume if the NewLines variable is true */

    while (Text[*Pos] && (Text[*Pos] == ' ' || Text[*Pos] == '\t' || Text[*Pos] == '\v' || Text[*Pos] == '\f' ||
                          ((Text[*Pos] == '\r' || Text[*Pos] == '\n') && NewLines))) (*Pos)++;
}

static Char8 *CfgConsumeString(const Char8 *Text, UIntN *Pos, Boolean Name) {
    /* The string characters are pretty much everything except for a quotation mark (not accepted even when escaped,
     * after all, you can't escape characters here lol.  */

    UIntN size = 10, cur = 0;
    Char8 *ret = EfiAllocateZeroPool(size + 1);

    if (ret == Null) return Null;

    while (Text[*Pos] && Text[*Pos] != '\r' && Text[*Pos] != '\n' && !(Name && Text[*Pos] == '"') &&
           !(!Name && Text[*Pos] == ':')) {
        /* Expand the buffer if needed (EfiReallocatePool uses EfiAllocateZeroPool under the hood, so no need to zero
         * end the string), and add the new character to it. */ 

        Char8 ch = Text[(*Pos)++];
    
        if (cur >= size) {
            Char8 *new = EfiReallocatePool(ret, size + 1, (size * 2) + 1);

            if (new == Null) {
                EfiFreePool(ret);
                return Null;
            }

            ret = new;
            size *= 2;
        }

        ret[cur++] = Name ? ch : EfiToLower(ch);
    }

    return ret;
}

static UIntN CfgConsumeNumber(const Char8 *Text, UIntN *Pos) {
    /* This is just converting a string into a number, which is simple (multiply the cur number by ten and add the next
     * number, making sure to subtract '0', so that it becomes a valid number). */

    UIntN ret = 0;
    while (Text[*Pos] >= '0' && Text[*Pos] <= '9') ret = (ret * 10) + Text[(*Pos)++] - '0';
    return ret;
}

static MenuEntryType CfgConsumeType(const Char8 *Text, UIntN *Pos) {
    /* The entry type is always an case insensitive string, that then we can just check against the known types. */

    Char8 *type = CfgConsumeString(Text, Pos, False);

    if (type == Null) return -1;
    else if (EfiCompareString8(type, "chicago")) {
        EfiFreePool(type);
        return MenuEntryCHicago;
    }

    /* And as you saw on type == Null, negative numbers mean that the entry is invalid. */

    EfiFreePool(type);

    return -1;
}

static Char16 *CfgConvertString(Char8 *Source) {
    /* The MenuAddEntry function expects the path to be a Char16 array (and CfgConsumeString returns a Char8 array),
     * but converting is easy. */

    if (Source == Null) return Null;

    UIntN size = 0;
    while (Source[size++]) ;
    
    Char16 *ret = EfiAllocateZeroPool((size + 1) * sizeof(Char16));
    if (ret != Null) for (UIntN i = 0; i < size; i++) ret[i] = (Char16)Source[i];

    EfiFreePool(Source);

    return ret;
}

EfiStatus CfgParse(Void) {
    /* First, open the config file, we can use EfiOpenFile (as it gets and opens the ESP volume if required),
     * EfiGetFileSize for the file size (duh, without it we would need to manually call GetInfo multiple times), and
     * then just allocate our little buffer and read the file (this time just using ->Read). */    

    UIntN size;
    EfiFile *file;
    EfiStatus status = EfiOpenFile(L"\\EFI\\CHOS\\BOOT.CFG", EFI_FILE_MODE_READ, &file);
    Char8 *cfg, *emsg = "The configuration file is corrupted/contains an invalid entry.";

    if (EFI_ERROR(status) || file == Null) {
        EfiDrawString("Failed to open the configuration file.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return status;
    } else if (!(size = EfiGetFileSize(file))) {
        EfiDrawString("Failed to read the configuration file (can't get the file size).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        file->Close(file);
        return EFI_DEVICE_ERROR;
    } else if ((cfg = EfiAllocateZeroPool(size + 1)) == Null) {
        EfiDrawString("Failed to read the configuration file (system is out of memory).", 5, EfiFont.Height + 15,
                      0xFF, 0xFF, 0xFF);
        file->Close(file);
        return EFI_OUT_OF_RESOURCES;
    } else if (EFI_ERROR((status = file->Read(file, &size, cfg)))) {
        EfiDrawString("Failed to read the configuration file.", 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        EfiFreePool(cfg);
        file->Close(file);
        return status;
    }

    /* We don't need the file anymore (as it's already in the buffer). */

    file->Close(file);

    for (UIntN i = 0; cfg[i]; i++) {
        /* Consume whitespaces, tabs, newlines, comments, etc, as those things are useless for us. */

        CfgConsumeWhiteSpaces(cfg, &i, True);

        if (!cfg[i]) break;
        else if (cfg[i] == '#') {
            /* Comments start with a hash symbol, and go until the next new line (or EOF, whichever comes first). */
            while (cfg[i] && cfg[i] != '\r' && cfg[i] != '\n') i++;
            continue;
        } else if (cfg[i++] != '"') {
            /* All the entries start with a quotation mark, so we can error out if it's some other character. */
            goto e;
        }

        /* Consume the entry name (stopping at the next quotation mark, or the EOF, whichever comes first, though this
         * time an EOF will error out). */

        Char8 *name = CfgConsumeString(cfg, &i, True);

        if (name == Null) {
            emsg = "The system is out of memory.";
            goto e;
        } else if (!cfg[i++]) {
            EfiFreePool(name);
            goto e;
        }

        /* Consume whitespaces again, BUT NOT NEWLINES THIS TIME (also, early EOF will error out now). */

        CfgConsumeWhiteSpaces(cfg, &i, False);

        if (cfg[i++] != '=') {
            EfiFreePool(name);
            goto e;
        }

        /* And again... */

        CfgConsumeWhiteSpaces(cfg, &i, False);

        /* Now we can consume the type. */

        MenuEntryType type = CfgConsumeType(cfg, &i);

        if (type < 0 || cfg[i++] != ':') {
            EfiFreePool(name);
            goto e;
        } else if (cfg[i++] != '"') {
            EfiFreePool(name);
            goto e;
        }

        Char16 *path = CfgConvertString(CfgConsumeString(cfg, &i, True));

        if (path == Null) {
            EfiFreePool(name);
            goto e;
        } else if (cfg[i++] != '"') {
            EfiFreePool(path);
            EfiFreePool(name);
            goto e;
        }

        /* Now, what we're going to do next for each type is different. */

        switch (type) {
        case MenuEntryCHicago: {
            /* CHicago: Read the image index if present. */

            if (!cfg[i] || cfg[i] == '\r' || cfg[i] == '\n') {
                MenuAddCHicagoEntry(name, path, 0);
                break;
            } else if (cfg[i] != ':' || cfg[i + 1] < '0' || cfg[i + 1] > '9') {
                EfiFreePool(path);
                EfiFreePool(name);
                goto e;
            }

            i++;
            MenuAddCHicagoEntry(name, path, CfgConsumeNumber(cfg, &i));

            break;
        }
        default: {
            /* Invalid entry. */
        
            EfiFreePool(path);
            EfiFreePool(name);
            
            emsg = "The entry type is invalid/unsupported.";
            
            goto e;
        }
        }

        EfiFreePool(path);
        EfiFreePool(name);

        if (cfg[i] && cfg[i] != '\r' && cfg[i] != '\n') goto e;
        else if (!cfg[i]) break;
    }

    goto c;

    /* Finally we can free the read buffer. If the user actually finished the code normally, we're going to take the
     * 'c' code path (which will just free the buffer and return success, else, the 'e' path, which frees it AND
     * print an error). */

e:  EfiDrawString(emsg, 5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
    EfiFreePool(cfg);
    return EFI_DEVICE_ERROR;
    
c:  EfiFreePool(cfg);
    return EFI_SUCCESS;
}
