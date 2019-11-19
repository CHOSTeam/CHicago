// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:11 BRT
// Last edited on November 11 of 2019, at 15:39 BRT

#include <chicago/process.h>

UIntPtr PsCreateThread(UIntPtr entry) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0A), "b"(entry));
	return ret;
}

UIntPtr PsGetTID(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0B));
	return ret;
}

UIntPtr PsGetPID(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0C));
	return ret;
}

Void PsSleep(UIntPtr ms) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x0D), "b"(ms));
}

UIntPtr PsWaitThread(UIntPtr id) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0E), "b"(id));
	return ret;
}

UIntPtr PsWaitProcess(UIntPtr id) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x0F), "b"(id));
	return ret;
}

Void PsLock(PLock lock) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x10), "b"((UIntPtr)lock));
}

Void PsUnlock(PLock lock) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x11), "b"((UIntPtr)lock));
}

Void PsExitThread(UIntPtr ret) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x12), "b"(ret));
}

Void PsExitProcess(UIntPtr ret) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x13), "b"(ret));
}
