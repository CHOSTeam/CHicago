// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:43 BRT
// Last edited on December 24 of 2019, at 13:45 BRT

#ifndef __CHICAGO_EXEC_H__
#define __CHICAGO_EXEC_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

IntPtr ExecCreateProcess(PWChar path);
IntPtr ExecLoadLibrary(PWChar path, Boolean global);
UIntPtr ExecGetSymbol(IntPtr handle, PWChar name);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_EXEC_H__

