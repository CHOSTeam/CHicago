// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:45 BRT
// Last edited on December 24 of 2019, at 13:44 BRT

#include <chicago/exec.h>

IntPtr ExecLoadLibrary(PWChar path, Boolean global) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x22), "b"((UIntPtr)path), "c"(global));
	return ret;
}

UIntPtr ExecGetSymbol(IntPtr handle, PWChar name) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x23), "b"(handle), "c"((UIntPtr)name));
	return ret;
}
