// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:55 BRT
// Last edited on January 05 of 2020, at 17:56 BRT

#include <chicago/process.h>

void __exit(int status) {
	PsExitProcess(status);
}
