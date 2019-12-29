// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 18:19 BRT
// Last edited on December 25 of 2019, at 20:34 BRT

#ifndef __CHICAGO_RAND_H__
#define __CHICAGO_RAND_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

Void RandSetSeed(UIntPtr seed);
UIntPtr RandGenerate(Void);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_RAND_H__
