// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 13:15 BRT
// Last edited on August 30 of 2019, at 13:57 BRT

#ifndef __CHICAGO_TYPES_H__
#define __CHICAGO_TYPES_H__

/* Base types */

#define Void void
#define PVoid void *
#define Char char
#define PChar char *
#define Short short
#define PShort short *
#define Int int
#define PInt int *
#define Long long
#define PLong long *
#define Float float
#define PFloat float *
#define Double double
#define PDouble double *

/* Unsigned types */

typedef unsigned Char UInt8, *PUInt8;
typedef unsigned Short UInt16, *PUInt16;
typedef unsigned Int UInt32, *PUInt32;
typedef unsigned Long ULong, *PULong;
typedef unsigned Long Long UInt64, *PUInt64;
typedef unsigned Short WChar, *PWChar;

/* Signed types */

typedef signed Char Int8, *PInt8;
typedef signed Short Int16, *PInt16;
typedef signed Int Int32, *PInt32;
typedef signed Long Long Int64, *PInt64;

/* Define our IntPtr type */

#ifdef ARCH_64
#define UINTPTR_MAX 0xFFFFFFFFFFFFFFFF
typedef unsigned Long Long UIntPtr, *PUIntPtr;
typedef signed Long Long IntPtr, *PIntPtr;
#else
#define UINTPTR_MAX 0xFFFFFFFF
typedef unsigned Int UIntPtr, *PUIntPtr;
typedef signed Int IntPtr, *PIntPtr;
#endif

/* Inline assembly */

#define Asm __asm__
#define Volatile __volatile__

/* Variadic arguments */

#define VariadicList __builtin_va_list
#define VariadicStart(v, l) __builtin_va_start(v, l)
#define VariadicEnd(v) __builtin_va_end(v)
#define VariadicArg(v, l) __builtin_va_arg(v, l)
#define VariadicCopy(d, s) __builtin_va_copy(d, s)

/* Attributes */

#define Packed __attribute__((packed))
#define Aligned(x) __attribute__((aligned(x)))
#define Optional __attribute__((weak))

/* Boolean and other defines */

typedef Char Boolean, *PBoolean;

#define True 1
#define False 0

#define Null ((PVoid)0)

#define Const const

#define TextifyMacro1(n) TextifyMacro2(#n)
#define TextifyMacro2(n) L##n
#define TextifyMacro3(n) #n
#define TextifyMacro(n) TextifyMacro1(n)
#define TextifyMacroC(n) TextifyMacro3(n)

#endif		// __CHICAGO_TYPES_H__
