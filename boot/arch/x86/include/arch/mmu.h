/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 22:45 BRT
 * Last edited on January 31 of 2021 at 12:39 BRT */

#pragma once

#include <efi/types.h>

#define MMU_PRESENT (1 << 0)
#define MMU_HUGE (1 << 7)
#define MMU_NO_EXEC (1ull << 63)
