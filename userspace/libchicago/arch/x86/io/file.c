// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:33 BRT
// Last edited on December 24 of 2019, at 13:43 BRT

#include <chicago/types.h>

IntPtr FsOpenFile(PWChar path) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x15), "b"((UIntPtr)path));
	return ret;
}

Boolean FsReadFile(IntPtr handle, UIntPtr len, PUInt8 buf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x16), "b"(handle), "c"(len), "d"((UIntPtr)buf));
	return ret;
}

Boolean FsWriteFile(IntPtr handle, UIntPtr len, PUInt8 buf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x17), "b"(handle), "c"(len), "d"((UIntPtr)buf));
	return ret;
}

Boolean FsMountFile(PWChar path, PWChar file, PWChar type) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x18), "b"((UIntPtr)path), "c"((UIntPtr)file), "d"((UIntPtr)type));
	return ret;
}

Boolean FsUmountFile(PWChar path) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x19), "b"((UIntPtr)path));
	return ret;
}

Boolean FsReadDirectoryEntry(IntPtr handle, UIntPtr entry, PWChar out) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1A), "b"(handle), "c"(entry), "d"((UIntPtr)out));
	return ret;
}

IntPtr FsFindInDirectory(IntPtr handle, PWChar name) {
	IntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1B), "b"(handle), "c"((UIntPtr)name));
	return ret;
}

Boolean FsCreateFile(IntPtr handle, PWChar name, UIntPtr type) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1C), "b"(handle), "c"((UIntPtr)name), "d"(type));
	return ret;
}

Boolean FsControlFile(IntPtr handle, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	Boolean ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1D), "b"(handle), "c"(cmd), "d"((UIntPtr)ibuf), "S"((UIntPtr)obuf));
	return ret;
}

UIntPtr FsGetFileSize(IntPtr handle) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1E), "b"(handle));
	return ret;
}

UIntPtr FsGetPosition(IntPtr handle) {
	UIntPtr ret;
	Asm Volatile("int $0x3F" : "=a"(ret) : "0"(0x1F), "b"(handle));
	return ret;
}

Void FsSetPosition(IntPtr handle, UIntPtr base, UIntPtr off) {
	Int discard;
	Asm Volatile("int $0x3F" : "=a"(discard) : "0"(0x20), "b"(handle), "c"(base), "d"(off));
}
