// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:43 BRT
// Last edited on December 31 of 2019, at 16:11 BRT

#ifndef __CHICAGO_EXEC_H__
#define __CHICAGO_EXEC_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

IntPtr ExecCreateProcess(PWChar path, UIntPtr argc, PWChar *argv);
IntPtr ExecLoadLibrary(PWChar path, Boolean global);
UIntPtr ExecGetSymbol(IntPtr handle, PWChar name);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_EXEC_H__

