// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 18:17 BRT
// Last edited on December 25 of 2019, at 20:33 BRT

#include <chicago/types.h>

UIntPtr RandSeed = 1;

Void RandSetSeed(UIntPtr seed) {
	RandSeed = seed;
}

UIntPtr RandGenerate(Void) {
	RandSeed = (RandSeed * 1103515245) + 12345;
	return RandSeed / 65536;
}
