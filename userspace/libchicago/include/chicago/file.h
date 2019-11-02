// File author is Ítalo Lima Marconato Matias
//
// Created on October 29 of 2018, at 19:33 BRT
// Last edited on October 29 of 2019, at 19:34 BRT

#ifndef __CHICAGO_FILE_H__
#define __CHICAGO_FILE_H__

#include <chicago/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

IntPtr FsOpenFile(PWChar path);
Void FsCloseFile(IntPtr file);
Boolean FsReadFile(IntPtr file, UIntPtr len, PUInt8 buf);
Boolean FsWriteFile(IntPtr file, UIntPtr len, PUInt8 buf);
Boolean FsMountFile(PWChar path, PWChar file, PWChar type);
Boolean FsUmountFile(PWChar path);
Boolean FsReadDirectoryEntry(IntPtr dir, UIntPtr entry, PWChar out);
IntPtr FsFindInDirectory(IntPtr dir, PWChar name);
Boolean FsCreateFile(IntPtr dir, PWChar name, UIntPtr type);
Boolean FsControlFile(IntPtr file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
UIntPtr FsGetFileSize(IntPtr file);
UIntPtr FsGetPosition(IntPtr file);
Void FsSetPosition(IntPtr file, UIntPtr base, UIntPtr off);

#ifdef __cplusplus
}
#endif

#endif		// __CHICAGO_FILE_H__

