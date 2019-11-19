// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:45 BRT
// Last edited on November 11 of 2019, at 15:41 BRT

#include <chicago/exec.h>

PExecHandle ExecLoadLibrary(PWChar path, Boolean global) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x22), "b"((UIntPtr)path), "c"(global));
	return (PExecHandle)ret;
}

Void ScExecCloseLibrary(PExecHandle handle) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x23), "b"((UIntPtr)handle));
}

UIntPtr ExecGetSymbol(PExecHandle handle, PWChar name) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x24), "b"((UIntPtr)handle), "c"((UIntPtr)name));
	return ret;
}
