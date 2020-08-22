/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 20 of 2020, at 15:35 BRT
 * Last edited on June 25 of 2020, at 07:49 BRT */

#include <iostream>
#include <sia.hxx>

int main(int argc, char **argv) {
	/* This program takes 3 arguments: the destination path, the kernel path, and the base directory path. */
	
	if (argc != 4) {
		cout << "Usage: " << argv[0] << " [dest] [kernel] [base]" << endl;
		return 1;
	}
	
	return !sia_create(argv[1], argv[2], argv[3]);
}
