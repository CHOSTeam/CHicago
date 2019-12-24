// File author is Ítalo Lima Marconato Matias
//
// Created on November 06 of 2019, at 18:44 BRT
// Last edited on December 24 of 2019, at 13:43 BRT

#include <chicago/types.h>

IntPtr ExecCreateProcess(PWChar path) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x21), "b"((UIntPtr)path));
	return ret;
}
