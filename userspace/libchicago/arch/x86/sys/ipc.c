// File author is Ítalo Lima Marconato Matias
//
// Created on December 24 of 2019, at 16:33 BRT
// Last edited on January 05 of 2020, at 22:04 BRT

#include <chicago/ipc.h>

Boolean IpcCreatePort(PWChar name) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x22), "b"((UIntPtr)name));
	return ret;
}

IntPtr IpcCreateResponsePort(Void) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x23));
	return ret;
}

Void IpcRemovePort(PWChar name) {
	IntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x24), "b"((UIntPtr)name));
}

Boolean IpcCheckPort(PWChar name) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x25), "b"((UIntPtr)name));
	return ret;
}

PIpcMessage IpcSendMessage(PWChar name, UInt32 msg, UIntPtr size, PUInt8 buf, IntPtr rport) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x26), "b"((UIntPtr)name), "c"(msg), "d"(size), "S"((UIntPtr)buf), "D"(rport));
	return (PIpcMessage)ret;
}

Void IpcSendResponse(IntPtr handle, UInt32 msg, UIntPtr size, PUInt8 buf) {
	IntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x27), "b"(handle), "c"(msg), "d"(size), "S"((UIntPtr)buf));
}

PIpcMessage IpcReceiveMessage(PWChar name) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x28), "b"((UIntPtr)name));
	return (PIpcMessage)ret;
}

PIpcMessage IpcReceiveResponse(IntPtr handle) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x29), "b"(handle));
	return (PIpcMessage)ret;
}
