// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on December 24 of 2019, at 13:49 BRT

#include <chicago/exec.h>
#include <chicago/process.h>

Void AppEntry(Void) {
	IntPtr guimngr = ExecCreateProcess(L"/System/Programs/guimngr.che");				// Let's try to init the GUI manager process
	
	if (guimngr == -1) {
		PsExitProcess((UIntPtr)-1);														// Failed, let's exit, this should make the kernel panic
	}
	
	PsWait(guimngr);																	// It shouldn't exit...
	PsExitProcess((UIntPtr)-1);															// Well... panic
}
