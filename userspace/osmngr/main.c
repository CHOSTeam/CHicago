// File author is Ítalo Lima Marconato Matias
//
// Created on October 31 of 2019, at 18:03 BRT
// Last edited on January 05 of 2020, at 18:47 BRT

#include <stdio.h>

int main(void) {
	const char str[] = "Test String";												// Define the test string
	
	setvbuf(stdout, NULL, _IOFBF, BUFSIZ);											// Create a new buffer for stdout, and set the buffering mode to full
	
	printf("Testing the buffering functions of my new libc...\n");					// Now, let's call printf some times...
	printf("Formatting some stuff:\n");
	printf("|1234567890123|\n");
	printf("|%13s|\n", str);
	printf("|%-13.9s|\n", str);
	printf("|%13.10s|\n", str);
	printf("|%13.11s|\n", str);
	printf("|%13.15s|\n", &str[2]);
	printf("|%13c|\n", str[5]);
	
	fflush(stdout);																	// And flush stdout!
	
	while (1) ;
}
