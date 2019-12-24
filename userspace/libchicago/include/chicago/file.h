// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:33 BRT
// Last edited on December 24 of 2019, at 13:46 BRT

#ifndef __CHICAGO_FILE_H__
#define __CHICAGO_FILE_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

IntPtr FsOpenFile(PWChar path);
Boolean FsReadFile(IntPtr handle, UIntPtr len, PUInt8 buf);
Boolean FsWriteFile(IntPtr handle, UIntPtr len, PUInt8 buf);
Boolean FsMountFile(PWChar path, PWChar file, PWChar type);
Boolean FsUmountFile(PWChar path);
Boolean FsReadDirectoryEntry(IntPtr handle, UIntPtr entry, PWChar out);
IntPtr FsFindInDirectory(IntPtr handle, PWChar name);
Boolean FsCreateFile(IntPtr handle, PWChar name, UIntPtr type);
Boolean FsControlFile(IntPtr handle, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
UIntPtr FsGetFileSize(IntPtr handle);
UIntPtr FsGetPosition(IntPtr handle);
Void FsSetPosition(IntPtr handle, UIntPtr base, UIntPtr off);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_FILE_H__

