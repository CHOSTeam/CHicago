// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 17:40 BRT
// Last edited on January 05 of 2020, at 17:53 BRT

#ifndef __CHLIBC_PLATFORM_H
#define __CHLIBC_PLATFORM_H

#include <chicago/types.h>

#define SEEK_SET 0x00
#define SEEK_CUR 0x01
#define SEEK_END 0x02

typedef IntPtr __platform_file_t;
typedef IntPtr __platform_lock_t;
typedef PWChar __platform_fname_t;

#endif		// __CHLIBC_PLATFORM_H
