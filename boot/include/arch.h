/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 23:21 BRT
 * Last edited on February 04 of 2021 at 12:29 BRT */

#pragma once

#include <chicago.h>
#include <menu.h>

UInt16 ArchGetFeatures(MenuEntryType);

SiaFile *ArchGetBestFitCHicago(SiaHeader*, UIntN, UInt16*);
EfiStatus ArchInitCHicagoMmu(UInt16, CHMapping**, Void**);
Void ArchJumpIntoCHicago(CHBootInfo*, UIntN, UIntN, UInt16);
