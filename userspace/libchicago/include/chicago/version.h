// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2019, at 18:45 BRT
// Last edited on October 31 of 2019, at 18:04 BRT

#ifndef __CHICAGO_VERSION_H__
#define __CHICAGO_VERSION_H__

#include <chicago/types.h>

typedef struct {
	PUInt32 major;
	PUInt32 minor;
	PUInt32 build;
	PWChar codename;
	PWChar arch;
} SystemVersion, *PSystemVersion;

#ifdef __cplusplus
extern "C"
{
#endif

Void SysGetVersion(PSystemVersion ver);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_VERSION_H__
