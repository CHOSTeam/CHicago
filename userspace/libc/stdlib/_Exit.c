// File author is Ítalo Lima Marconato Matias
//
// Created on December 31 of 2019, at 17:16 BRT
// Last edited on January 03 of 2019, at 21:06 BRT

#include <chicago/process.h>

_Noreturn void _Exit(int status) {
	PsExitProcess(status);										// Redirect to PsExitProcess
	__builtin_unreachable();
}
