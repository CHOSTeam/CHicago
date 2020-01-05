// File author is Ítalo Lima Marconato Matias
//
// Created on December 31 of 2019, at 17:15 BRT
// Last edited on December 31 of 2019, at 17:16 BRT

void (*__exit_handlers[32])(void);
unsigned int __exit_handler_ptr = 32;

int atexit(void (*func)(void)) {
	if (__exit_handler_ptr == 0) {								// The "stack" pointer is already at zero? (That is, we already registered the max amount of handlers?)
		return -1;												// Yes, so return -1 (error)
	}
	
	__exit_handlers[--__exit_handler_ptr] = func;				// Register our function and move the "stack" pointer down
	
	return 0;
}
