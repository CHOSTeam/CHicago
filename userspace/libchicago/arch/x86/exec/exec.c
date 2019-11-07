// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:44 BRT
// Last edited on November 06 of 2019, at 18:45 BRT

#include <chicago/types.h>

UIntPtr ExecCreateProcess(PWChar path) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x22), "b"((UIntPtr)path));
	return ret;
}
