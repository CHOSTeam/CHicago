// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2019, at 18:45 BRT
// Last edited on December 30 of 2019, at 15:26 BRT

#ifndef __CHICAGO_MISC_H__
#define __CHICAGO_MISC_H__

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
PInt SysGetErrno(Void);
Void SysCloseHandle(IntPtr handle);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_MISC_H__
