// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:43 BRT
// Last edited on November 06 of 2019, at 18:43 BRT

#ifndef __CHICAGO_EXEC_H__
#define __CHICAGO_EXEC_H__

#include <chicago/types.h>

typedef struct {
	UIntPtr todo[6];
} ExecHandle, *PExecHandle;

#ifdef __cplusplus
extern "C"
{
#endif

UIntPtr ExecCreateProcess(PWChar path);
PExecHandle ExecLoadLibrary(PWChar path, Boolean global);
Void ScExecCloseLibrary(PExecHandle handle);
UIntPtr ExecGetSymbol(PExecHandle handle, PWChar name);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_EXEC_H__

