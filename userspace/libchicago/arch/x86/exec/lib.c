// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:45 BRT
// Last edited on January 01 of 2020, at 19:09 BRT

#include <chicago/exec.h>

IntPtr ExecLoadLibrary(PWChar path, Boolean global) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x24), "b"((UIntPtr)path), "c"(global));
	return ret;
}

UIntPtr ExecGetSymbol(IntPtr handle, PWChar name) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x25), "b"(handle), "c"((UIntPtr)name));
	return ret;
}
