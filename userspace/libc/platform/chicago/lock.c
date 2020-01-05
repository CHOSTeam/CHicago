// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:52 BRT
// Last edited on January 05 of 2020, at 18:21 BRT

#include <chicago/misc.h>
#include <chicago/process.h>

#include <chlibc/platform.h>

__platform_lock_t __create_lock(void) {
	return PsCreateLock();
}

void __remove_lock(__platform_lock_t lock) {
	SysCloseHandle(lock);
}

void __lock(__platform_lock_t lock) {
	PsLock(lock);
}

int __try_lock(__platform_lock_t lock) {
	return PsTryLock(lock);
}

void __unlock(__platform_lock_t lock) {
	PsUnlock(lock);
}
