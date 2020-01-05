// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 15:17 BRT
// Last edited on January 01 of 2020, at 15:47 BRT

#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef NDEBUG
#define assert(expr) (void)((expr) || __assert(#expr, __FILE__, __LINE__), 0)
#else
#define assert(ignore) ((void)0)
#endif

#define static_assert _Static_assert

#ifdef __cplusplus
extern "C"
{
#endif

void __assert(const char *msg, const char *filename, int line);

#ifdef __cplusplus
}
#endif

#endif		// _ASSERT_H
