// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on December 25 of 2019, at 18:19 BRT

#include <chicago/exec.h>
#include <chicago/process.h>

static Void RunGuimngr(Void) {
	IntPtr handle = ExecCreateProcess(L"/System/Programs/guimngr.che");					// Try to create the process
	
	if (handle == -1) {
		PsExitProcess((UIntPtr)-1);														// Failed, this is a critical process, so we should exit and make the kernel panic
	}
	
	PsWait(handle);																		// It shouldn't exit..
	PsExitProcess((UIntPtr)-1);															// Well... panic
}

Void AppEntry(Void) {
	if (PsCreateThread((UIntPtr)RunGuimngr) == -1) {									// Run the graphical manager
		PsExitProcess((UIntPtr)-1);
	}
	
	while (True) ;
}
