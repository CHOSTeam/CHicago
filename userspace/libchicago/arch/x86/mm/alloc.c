// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 18:53 BRT
// Last edited on December 24 of 2019, at 13:38 BRT

#include <chicago/types.h>

UIntPtr MmAllocMemory(UIntPtr size) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x02), "b"(size));
	return ret;
}

Void MmFreeMemory(UIntPtr block) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x03), "b"(block));
}

UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x04), "b"(block), "c"(size));
	return ret;
}

UIntPtr MmGetUsage(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x05));
	return ret;
}
