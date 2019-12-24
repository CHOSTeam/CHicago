// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 18:40 BRT
// Last edited on December 24 of 2019, at 13:47 BRT

#include <chicago/misc.h>

Void SysGetVersion(PSystemVersion ver) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x00), "b"((UIntPtr)ver));
}

Void SysCloseHandle(IntPtr handle) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x01), "b"(handle));
}
