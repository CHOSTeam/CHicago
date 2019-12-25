// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 18:12 BRT
// Last edited on December 25 of 2019, at 18:12 BRT

#ifndef __CHICAGO_SHM_H__
#define __CHICAGO_SHM_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

UIntPtr ShmCreateSection(UIntPtr size, PUIntPtr key);
UIntPtr ShmMapSection(UIntPtr key);
Void ShmUnmapSection(UIntPtr key);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_SHM_H__


