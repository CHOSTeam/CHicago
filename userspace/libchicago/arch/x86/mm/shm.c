// File author is Ítalo Lima Marconato Matias
//
// Created on December 25 of 2019, at 17:42 BRT
// Last edited on December 25 of 2019, at 18:13 BRT

#include <chicago/types.h>

UIntPtr ShmCreateSection(UIntPtr size, PUIntPtr key) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x2B), "b"(size), "c"(key));
	return ret;
}

UIntPtr ShmMapSection(UIntPtr key) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x2C), "b"(key));
	return ret;
}

Void ShmUnmapSection(UIntPtr key) {
	IntPtr discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x2D), "b"(key));
}
