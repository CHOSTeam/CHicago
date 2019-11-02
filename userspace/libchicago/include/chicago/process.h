// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:12 BRT
// Last edited on October 29 of 2019, at 19:34 BRT

#ifndef __CHICAGO_PROCESS_H__
#define __CHICAGO_PROCESS_H__

#include <chicago/types.h>

typedef struct {
	UIntPtr todo[2];
} Lock, *PLock;

#ifdef __cplusplus
extern "C"
{
#endif

UIntPtr PsCreateThread(UIntPtr entry);
UIntPtr PsGetTID(Void);
UIntPtr PsGetPID(Void);
Void PsSleep(UIntPtr ms);
UIntPtr PsWaitThread(UIntPtr id);
UIntPtr PsWaitProcess(UIntPtr id);
Void PsLock(PLock lock);
Void PsUnlock(PLock lock);
Void PsExitThread(UIntPtr ret);
Void PsExitProcess(UIntPtr ret);
Void PsForceSwitch(Void);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_PROCESS_H__
