// File author is Ítalo Lima Marconato Matias
//
// Created on December 24 of 2019, at 16:39 BRT
// Last edited on December 25 of 2019, at 22:20 BRT

#ifndef __CHICAGO_IPC_H__
#define __CHICAGO_IPC_H__

#include <chicago/types.h>

typedef struct {
	UInt32 msg;
	IntPtr src;
	IntPtr rport;
	UIntPtr size;
	PUInt8 buffer;
} IpcMessage, *PIpcMessage;

#ifdef __cplusplus
extern "C"
{
#endif

Boolean IpcCreatePort(PWChar name);
IntPtr IpcCreateResponsePort(Void);
Void IpcRemovePort(PWChar name);
Boolean IpcCheckPort(PWChar name);
PIpcMessage IpcSendMessage(PWChar name, UInt32 msg, UIntPtr size, PUInt8 buf, IntPtr rport);
Void IpcSendResponse(IntPtr handle, UInt32 msg, UIntPtr size, PUInt8 buf);
PIpcMessage IpcReceiveMessage(PWChar name);
PIpcMessage IpcReceiveResponse(IntPtr handle);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_IPC_H__
