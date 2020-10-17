/* File author is Ítalo Lima Marconato Matias
 *
 * Created on August 12 of 2020, at 08:57 BRT
 * Last edited on October 17 of 2020, at 13:04 BRT */

#include <chicago/driver.hxx>

extern "C" Status DrvEntry(const KernelInterface *Kernel) {
	/* Small test, just return. */
	
	if (Kernel == Null) {
		return Status::InvalidArg;
	}
	
	return Status::Success;
}
