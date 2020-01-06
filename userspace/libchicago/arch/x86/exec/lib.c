// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:45 BRT
// Last edited on January 05 of 2020, at 22:03 BRT

#include <chicago/exec.h>

IntPtr ExecLoadLibrary(PWChar path, Boolean global) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x20), "b"((UIntPtr)path), "c"(global));
	return ret;
}

UIntPtr ExecGetSymbol(IntPtr handle, PWChar name) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x21), "b"(handle), "c"((UIntPtr)name));
	return ret;
}
