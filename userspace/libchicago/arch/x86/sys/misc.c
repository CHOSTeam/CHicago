// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 18:40 BRT
// Last edited on December 30 of 2019, at 10:21 BRT

#include <chicago/misc.h>

Void SysGetVersion(PSystemVersion ver) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x00), "b"((UIntPtr)ver));
}

PInt SysGetErrno(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x01));
	return (PInt)ret;
}

Void SysCloseHandle(IntPtr handle) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x02), "b"(handle));
}
