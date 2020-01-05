// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2019, at 10:59 BRT
// Last edited on January 03 of 2020, at 11:01 BRT

void (*__quick_exit_handlers[32])(void);
unsigned int __quick_exit_handler_ptr = 32;

int at_quick_exit(void (*func)(void)) {
	if (__quick_exit_handler_ptr == 0) {								// The "stack" pointer is already at zero? (That is, we already registered the max amount of handlers?)
		return -1;														// Yes, so return -1 (error)
	}
	
	__quick_exit_handlers[--__quick_exit_handler_ptr] = func;			// Register our function and move the "stack" pointer down
	
	return 0;
}
