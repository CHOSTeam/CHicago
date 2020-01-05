// File author is Ítalo Lima Marconato Matias
//
// Created on December 31 of 2019, at 14:58 BRT
// Last edited on December 31 of 2019, at 22:05 BRT

#include <chicago/types.h>

#include <stdlib.h>

extern void _init(void);
extern void __libc_init_streams(void);

extern int main(int argc, char **argv);

Void AppEntry(UIntPtr argc, PWChar *argv, PChar *cargv) {
	(void)argv;							// We're not going to use the unicode argv
	
	_init();							// Call the global initializers
	__libc_init_streams();				// Init the streams
	
	exit(main(argc, cargv));			// call the main function, and pass the return value into exit()
}
