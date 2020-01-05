// File author is Ítalo Lima Marconato Matias
//
// Created on December 31 of 2019, at 17:16 BRT
// Last edited on January 03 of 2019, at 10:57 BRT

#include <stdlib.h>

extern void (*__exit_handlers[32])(void);
extern unsigned int __exit_handler_ptr;

_Noreturn void exit(int status) {
	while (__exit_handler_ptr < 32) {							// Execute all the registered handlers
		__exit_handlers[__exit_handler_ptr++]();
	}
	
	_Exit(status);												// And exit
}
