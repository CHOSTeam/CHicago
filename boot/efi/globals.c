/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 02 of 2021, at 17:29 BRT
 * Last edited on February 15 of 2021 at 23:54 BRT */

#include <efi/gop.h>
#include <efi/loaded_image.h>
#include <efi/rng.h>

/* ImageHandle and SystemTable are passed to the entry function, and redirected to us, RuntimeServices, BootServices and
 * ConfigTables are the ->RS, ->BS and ->CS fields from the SystemTable. */

EfiHandle EfiIH = Null;
const EfiSystemTable *EfiST = Null;
const EfiRuntimeServices *EfiRS = Null;
const EfiBootServices *EfiBS = Null;
const EfiConfigurationTable *EfiCT = Null;

/* Accessors to the ConIn/ConOut/ConErr interfaces, just so that we don't need to write SystemTable->ConIn/Out/Err every
 * time. */

EfiSimpleTextInput *EfiConIn = Null;
EfiSimpleTextOutput *EfiConOut = Null, *EfiConErr = Null;

EfiGraphicsOutput *EfiGop = Null;

/* Global protocol GUID values. */

EfiGuid EfiDevicePathGuid = EFI_DEVICE_PATH_GUID;
EfiGuid EfiSimpleTextInputExGuid = EFI_SIMPLE_TEXT_INPUT_EX_GUID;
EfiGuid EfiSimpleTextInputGuid = EFI_SIMPLE_TEXT_INPUT_GUID;
EfiGuid EfiSimpleTextOutputGuid = EFI_SIMPLE_TEXT_OUTPUT_GUID;
EfiGuid EfiSimplePointerGuid = EFI_SIMPLE_POINTER_GUID;
EfiGuid EfiAbsolutePointerGuid = EFI_ABSOLUTE_POINTER_GUID;
EfiGuid EfiSimpleFileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_GUID;
EfiGuid EfiFileInfoGuid = EFI_FILE_INFO_GUID;
EfiGuid EfiFileSystemInfoGuid = EFI_FILE_SYSTEM_INFO_GUID;
EfiGuid EfiFileSystemLabelGuid = EFI_FILE_SYSTEM_LABEL_GUID;
EfiGuid EfiGraphicsOutputGuid = EFI_GRAPHICS_OUTPUT_GUID;
EfiGuid EfiLoadedImageGuid = EFI_LOADED_IMAGE_GUID;
EfiGuid EfiRngGuid = EFI_RNG_GUID;

EfiStatus EfiInitLib(EfiHandle IH, const EfiSystemTable *ST, Boolean DisableWatchdog) {
    EfiIH = IH;
    EfiST = ST;
    EfiRS = ST->RS;
    EfiBS = ST->BS;
    EfiCT = ST->CT;

    EfiConIn = ST->ConIn;
    EfiConOut = ST->ConOut;
    EfiConErr = ST->ConErr;

    /* If the caller is a bootloader/boot manager (not just a normal application), it will probably want to disable the
     * watchdog (so that it has all the time in the world to load everything up), so let's do that (if we were asked
     * to). */
    
    if (DisableWatchdog) {
        EfiBS->SetWatchdogTimer(0, 0, 0, Null);
    }
    
    return EfiBS->LocateProtocol(&EfiGraphicsOutputGuid, Null, (Void**)&EfiGop);
}
