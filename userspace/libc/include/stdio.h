// File author is Ítalo Lima Marconato Matias
//
// Created on January 01 of 2020, at 16:56 BRT
// Last edited on January 05 of 2020, at 13:00 BRT

#ifndef _STDIO_H
#define _STDIO_H

#include <chicago/types.h>

#include <stdarg.h>
#include <stddef.h>

#ifdef __STDIO__
#define __FLAGS_EOF 0x08
#define __FLAGS_ERROR 0x10
#define __FLAGS_READ 0x20
#define __FLAGS_WRITE 0x40
#define __FLAGS_APPEND 0x80
#define __FLAGS_UPDATE 0x100
#define __FLAGS_EXCLUSIVE 0x200
#define __FLAGS_READING 0x400
#define __FLAGS_WRITING 0x800
#endif

#define EOF (-1)

#define BUFSIZ 512
#define _IOFBF 0x01
#define _IOLBF 0x02
#define _IONBF 0x04

#define SEEK_SET 0x00
#define SEEK_CUR 0x01
#define SEEK_END 0x02

#define getc_unlocked(stream) fgetc_unlocked(stream)
#define getc(stream) fgetc(stream)
#define putc_unlocked(c, stream) fputc_unlocked(c, stream)
#define putc(c, stream) fputc(c, stream)

typedef long int fpos_t;

typedef struct {
	IntPtr file;
	IntPtr lock;
	PWChar filename;
	char *buf;
	int buf_free;
	size_t buf_size;
	size_t buf_read;
	size_t buf_pos;
	fpos_t pos;
	int unget;
	int flags;
} FILE;

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __STDIO__
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
#endif

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
FILE *freopen(const char *restrict filename, const char *restrict mode, FILE *restrict stream);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fflush(FILE *stream);
int fgetc(FILE *stream);
int fgetpos(FILE *restrict stream, fpos_t *restrict pos);
char *fgets(char *restrict s, int n, FILE *restrict stream);
void flockfile(FILE *stream);
int fprintf(FILE *restrict stream, const char *restrict format, ...);
int fputc(int c, FILE *stream);
int fputs(const char *restrict s, FILE *restrict stream);
size_t fread(void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream);
int fseek(FILE *stream, long int offset, int whence);
int fsetpos(FILE *stream, const fpos_t *pos);
long int ftell(FILE *stream);
int ftrylockfile(FILE *stream);
void funlockfile(FILE *stream);
size_t fwrite(const void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream);
int getchar(void);
int putchar(int c);
int printf(const char *restrict format, ...);
int puts(const char *s);
void rewind(FILE *stream);
int setbuf(FILE *restrict stream, char *restrict buf);
int setvbuf(FILE *restrict stream, char *restrict buf, int mode, size_t size);
int snprintf(char *restrict s, size_t n, const char *restrict format, ...);
int sprintf(char *restrict s, const char *restrict format, ...);
int ungetc(int c, FILE *stream);
int vfprintf(FILE *restrict stream, const char *restrict format, va_list arg);
int vprintf(const char *restrict format, va_list arg);
int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list arg);
int vsprintf(char *restrict s, const char *restrict format, va_list arg);

void clearerr_unlocked(FILE *stream);
int feof_unlocked(FILE *stream);
int ferror_unlocked(FILE *stream);
int fflush_unlocked(FILE *stream);
int fgetc_unlocked(FILE *stream);
int fgetpos_unlocked(FILE *restrict stream, fpos_t *restrict pos);
char *fgets_unlocked(char *restrict s, int n, FILE *restrict stream);
int fprintf_unlocked(FILE *restrict stream, const char *restrict format, ...);
int fputc_unlocked(int c, FILE *stream);
int fputs_unlocked(const char *restrict s, FILE *restrict stream);
size_t fread_unlocked(void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream);
int fseek_unlocked(FILE *stream, long int offset, int whence);
int fsetpos_unlocked(FILE *stream, const fpos_t *pos);
long int ftell_unlocked(FILE *stream);
size_t fwrite_unlocked(const void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream);
int getchar_unlocked(void);
int printf_unlocked(const char *restrict format, ...);
int putchar_unlocked(int c);
int puts_unlocked(const char *s);
void rewind_unlocked(FILE *stream);
int setbuf_unlocked(FILE *restrict stream, char *restrict buf);
int setvbuf_unlocked(FILE *restrict stream, char *restrict buf, int mode, size_t size);
int ungetc_unlocked(int c, FILE *stream);
int vfprintf_unlocked(FILE *restrict stream, const char *restrict format, va_list arg);
int vprintf_unlocked(const char *restrict format, va_list arg);

#ifdef __cplusplus
}
#endif

#endif		// _STDIO_H
