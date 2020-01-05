// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:33 BRT
// Last edited on January 04 of 2020, at 18:03 BRT

#include <chicago/types.h>

IntPtr FsOpenFile(PWChar path) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x17), "b"((UIntPtr)path));
	return ret;
}

UIntPtr FsReadFile(IntPtr handle, UIntPtr len, PUInt8 buf) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x18), "b"(handle), "c"(len), "d"((UIntPtr)buf));
	return ret;
}

UIntPtr FsWriteFile(IntPtr handle, UIntPtr len, PUInt8 buf) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x19), "b"(handle), "c"(len), "d"((UIntPtr)buf));
	return ret;
}

Boolean FsMountFile(PWChar path, PWChar file, PWChar type) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1A), "b"((UIntPtr)path), "c"((UIntPtr)file), "d"((UIntPtr)type));
	return ret;
}

Boolean FsUmountFile(PWChar path) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1B), "b"((UIntPtr)path));
	return ret;
}

Boolean FsReadDirectoryEntry(IntPtr handle, UIntPtr entry, PWChar out) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1C), "b"(handle), "c"(entry), "d"((UIntPtr)out));
	return ret;
}

IntPtr FsFindInDirectory(IntPtr handle, PWChar name) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1D), "b"(handle), "c"((UIntPtr)name));
	return ret;
}

Boolean FsCreateFile(IntPtr handle, PWChar name, UIntPtr type) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1E), "b"(handle), "c"((UIntPtr)name), "d"(type));
	return ret;
}

Boolean FsControlFile(IntPtr handle, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1F), "b"(handle), "c"(cmd), "d"((UIntPtr)ibuf), "S"((UIntPtr)obuf));
	return ret;
}

UIntPtr FsGetFileSize(IntPtr handle) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x20), "b"(handle));
	return ret;
}

UIntPtr FsGetPosition(IntPtr handle) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x21), "b"(handle));
	return ret;
}

Void FsSetPosition(IntPtr handle, UIntPtr base, UIntPtr off) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x22), "b"(handle), "c"(base), "d"(off));
}
