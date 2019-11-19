// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on November 16 of 2019, at 11:30 BRT

#include <chicago/exec.h>
#include <chicago/process.h>

Void AppEntry(Void) {
	UIntPtr guimngr = ExecCreateProcess(L"/System/Programs/guimngr.che");				// Let's try to init the GUI manager process
	
	if (guimngr == 0) {
		PsExitProcess((UIntPtr)-1);														// Failed, let's exit, this should make the kernel panic
	}
	
	PsWaitProcess(guimngr);																// It shouldn't exit...
	PsExitProcess((UIntPtr)-1);															// Well... panic
}
