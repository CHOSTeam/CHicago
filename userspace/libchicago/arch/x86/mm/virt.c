// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 18:58 BRT
// Last edited on January 05 of 2020, at 22:01 BRT

#include <chicago/types.h>

UIntPtr VirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x03), "b"(addr), "c"(size), "d"(flags));
	return ret;
}

Boolean VirtFreeAddress(UIntPtr addr, UIntPtr size) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x04), "b"(addr), "c"(size));
	return ret;
}

UInt32 VirtQueryProtection(UIntPtr addr) {
	UInt32 ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x05), "b"(addr));
	return ret;
}

Boolean VirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x06), "b"(addr), "c"(size), "d"(flags));
	return ret;
}

UIntPtr VirtGetUsage(Void) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x07));
	return ret;
}
