// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:11 BRT
// Last edited on December 24 of 2019, at 13:40 BRT

#include <chicago/process.h>

IntPtr PsCreateThread(UIntPtr entry) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0B), "b"(entry));
	return ret;
}

IntPtr PsGetCurrentThread(Void) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0C));
	return ret;
}

IntPtr PsGetCurrentProcess(Void) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0D));
	return ret;
}

Void PsSleep(UIntPtr ms) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0E), "b"(ms));
}

UIntPtr PsWait(IntPtr handle) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0F), "b"(handle));
	return ret;
}

IntPtr PsCreateLock(Void) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x10));
	return ret;
}

Void PsLock(IntPtr handle) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x11), "b"(handle));
}

Void PsUnlock(IntPtr handle) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x12), "b"(handle));
}

Void PsExitThread(UIntPtr ret) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x13), "b"(ret));
}

Void PsExitProcess(UIntPtr ret) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x14), "b"(ret));
}
