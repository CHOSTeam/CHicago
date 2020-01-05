// File author is Ítalo Lima Marconato Matias
//
// Created on December 31 of 2019, at 17:16 BRT
// Last edited on January 05 of 2020, at 17:22 BRT

#include <chlibc/platform.h>

_Noreturn void _Exit(int status) {
	__exit(status);										// Redirect...
	__builtin_unreachable();
}
