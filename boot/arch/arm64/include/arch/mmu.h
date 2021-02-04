/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 12:52 BRT
 * Last edited on February 04 04 of 2021 at 12:37 BRT */

#pragma once

#include <efi/types.h>

#define MMU_PRESENT (1 << 0)
#define MMU_TABLE (1 << 1)
#define MMU_DEVICE (2 << 2)
#define MMU_READ_ONLY (1 << 7)
#define MMU_OUTER_SHARE (2 << 8)
#define MMU_INNER_SHARE (3 << 8)
#define MMU_ACCESS (1 << 10)
#define MMU_NO_EXEC (1ull << 53) | (1ull << 54)
