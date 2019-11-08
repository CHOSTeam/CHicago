// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on November 08 of 2019, at 18:28 BRT

#include <chicago/file.h>

Void AppEntry(Void) {
	WChar str[] = L"Hello from /System/Programs/osmngr.che!";				// The string that we are going to write
	IntPtr con = FsOpenFile(L"/Devices/Console");							// Try to open the console file
	
	if (con == -1) {
		goto h;																// Failed...
	}
	
	FsWriteFile(con, sizeof(str), (PUInt8)str);								// Write the string to the screen!
	
h:	while (True) ;															// And, for now, as we don't have anything to do, halt
}
