// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 17:42 BRT
// Last edited on January 05 of 2020, at 22:04 BRT

#include <chicago/types.h>

UIntPtr ShmCreateSection(UIntPtr size, PUIntPtr key) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x2A), "b"(size), "c"(key));
	return ret;
}

UIntPtr ShmMapSection(UIntPtr key) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x2B), "b"(key));
	return ret;
}

Void ShmUnmapSection(UIntPtr key) {
	IntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x2C), "b"(key));
}
