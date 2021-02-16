/* File author is Ítalo Lima Marconato Matias
 *
 * Created on February 15 of 2021, at 23:42 BRT
 * Last edited on February 16 of 2021 at 00:02 BRT */

#pragma once

#include <efi/types.h>

#define EFI_RNG_GUID { 0x3152BCA5, 0xEADE, 0x433D, \
                       { 0x86, 0x2E, 0xC0, 0x1C, 0xDC, 0x29, 0x1F, 0x44 } }

#define EFI_RNG_ALGORITHM_SP800_90_HASH_256_GUID { 0xA7AF67CB, 0x603B, 0x4D42, \
                                                   { 0xBA, 0x21, 0x70, 0xBF, 0xB6, 0x29, 0x3F, 0x96 } }
#define EFI_RNG_ALGORITHM_SP800_90_HMAC_256_GUID { 0xC5149B43, 0xAE85, 0x4F53, \
                                                   { 0x99, 0x82, 0xB9, 0x43, 0x35, 0xD3, 0xA9, 0xe7 } }
#define EFI_RNG_ALGORITHM_SP800_90_CTR_256_GUID { 0x44F0DE6E, 0x4D8C, 0x4045, \
                                                  { 0xA8, 0xC7, 0x4D, 0xD1, 0x68, 0x85, 0x6B, 0x9E } }
#define EFI_RNG_ALGORITHM_X9_31_3DES_GUID { 0x63C4785A, 0xCA34, 0x4012, \
                                            { 0xA3, 0xC8, 0x0B, 0x6A, 0x32, 0x4F, 0x55, 0x46 } }
#define EFI_RNG_ALGORITHM_X9_31_AES_GUID { 0xACD03321, 0x777E, 0x4D3D, \
                                           { 0xB1, 0xC8, 0x20, 0xCF, 0xD8, 0x88, 0x20, 0xC9 } }
#define EFI_RNG_ALGORITHM_RAW { 0xE43176D7, 0xB6E8, 0x4827, \
                                { 0xB7, 0x84, 0x7F, 0xFD, 0xC4, 0xB6, 0x85, 0x61 } }

typedef struct _EfiRng EfiRng;

typedef EfiStatus (*_EfiRngGetInfo)(EfiRng*, UIntN*, EfiGuid*);
typedef EfiStatus (*_EfiRngGetRandom)(EfiRng*, EfiGuid*, UIntN, UInt8*);

struct _EfiRng {
    _EfiRngGetInfo GetInfo;
    _EfiRngGetRandom GetRandom;
};
