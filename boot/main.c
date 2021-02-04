/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 01 of 2021, at 19:11 BRT
 * Last edited on January 25 of 2021 at 15:44 BRT */

#include <efi/lib.h>
#include <menu.h>

EfiStatus EfiMain(EfiHandle IH, EfiSystemTable *ST) {
    /* First, initialize the Efi library functions, and setup the graphics (+ clean the screen into the right bg
     * color). */

    EfiStatus status = EfiInitLib(IH, ST, True);

    if (EFI_ERROR(status) || EFI_ERROR((status = EfiSetupGraphics(0x00, 0x00, 0x00)))) {
        return status;
    }
    
    /* Parse the config file, announce that we're inside the boot manager, and go to the main menu. */

    MenuStart();

    while (True) ;
}
