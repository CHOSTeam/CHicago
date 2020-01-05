// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2019, at 11:00 BRT
// Last edited on January 03 of 2020, at 11:02 BRT

#include <stdlib.h>

extern void (*__quick_exit_handlers[32])(void);
extern unsigned int __quick_exit_handler_ptr;

_Noreturn void quick_exit(int status) {
	while (__quick_exit_handler_ptr < 32) {								// Execute all the registered handlers
		__quick_exit_handlers[__quick_exit_handler_ptr++]();
	}
	
	_Exit(status);														// And exit
}
