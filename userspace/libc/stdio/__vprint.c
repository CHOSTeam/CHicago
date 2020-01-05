// File author is Ítalo Lima Marconato Matias
//
// Created on January 03 of 2020, at 10:44 BRT
// Last edited on January 04 of 2020, at 21:56 BRT

#define __STDIO__
#include <stdio.h>
#include <string.h>

typedef struct {
	va_list arg;
} va_list_wrapper;

static size_t __parse_flags(const char *restrict format, va_list_wrapper arg, int *lj, int *fs, int *alt, int *zero, int *fw, int *pr, int *fwset, int *prset) {
	size_t ret = 0;
	
s:	switch (*format) {																							// Let's parse the flags!
	case '-': {																									// Left-justified instead of right-justified
		*lj = 1;
		ret++;
		format++;
		goto s;
	}
	case '+': {																									// Always append the sign
		*fs = 1;
		ret++;
		format++;
		goto s;
	}
	case '#': {																									// Alternative form
		*alt = 1;
		ret++;
		format++;
		goto s;
	}
	case '0': {																									// Pad with zeroes
		*zero = 1;
		ret++;
		format++;
		goto s;
	}
	case ' ': {																									// This have two functions...
		if (*fs != 1) {																							// First, in case we haven't already used the '+' flag, we set to use a space instead of the (positive) sign itself...
			*fs = 2;
		}
		
		if (*zero != 1) {																						// Also we can set the space-pad flag (if it isn't set to zeroes already)
			*zero = 0;
		}
		
		ret++;
		format++;
		
		goto s;
	}
	case '*': {																									// Set the field width using the args
		if (*fwset) {
			ret = 0;
			break;
		}
		
		*fw = va_arg(arg.arg, int);
		*fwset = 1;
		ret++;
		format++;
		
		goto s;
	}
	case '.': {																									// Set the precision
		if (*prset) {																							// Make sure that we haven't set it before
			ret = 0;
			break;
		}
		
		ret++;
		format++;																								// Increase the pointer
		
		if (*format >= '0' && *format <= '9') {																	// Set using args?
			while (*format >= '0' && *format <= '9') {															// No...
				*pr = (*pr * 10) + (*format++ - '0');
				ret++;
			}
			
			*prset = 1;
		} else if (*format == '*') {
			*pr = va_arg(arg.arg, int);																			// Yes
			*prset = 1;
			ret++;
			format++;
		}
		
		goto s;
	}
	default: {																									// Not a flag... or is it?
		if ((*format >= '0' && *format <= '9') && !*fwset) {													// Set the field width?
			while (*format >= '0' && *format <= '9') {															// Yes, do it!
				*fw = (*fw * 10) + (*format++ - '0');
				ret++;
			}
			
			*fwset = 1;
		} else {
			break;
		}
		
		goto s;
	}
	}
	
	return ret;
}

static char *__write_char(int *err, int usestream, FILE *restrict stream, int limitn, char *restrict s, size_t n, int pos, char c) {
	if (!usestream && !(limitn && (pos + 1) >= (int)n)) {														// Write to the string?
		*s++ = c;																								// Yes!
		*err = 0;
	} else if (usestream) {																						// Use fputc?
		*err = fputc_unlocked(c, stream) == EOF;																// Yes!
	}
	
	return s;
}

static char *__write_string(int *err, int usestream, FILE *restrict stream, int limitn, char *restrict s, size_t n, int pos, const char *restrict str) {
	if (usestream) {																							// Can we just use fputs?
		*err = fputs_unlocked(str, stream) == EOF;																// Yes!
	} else {
		while (*str) {																							// No, so let's just call __write_char for each character in the string...
			s = __write_char(err, usestream, stream, limitn, s, n, pos++, *str++);
		}
	}
	
	return s;
}

static char *__write_int(char *restrict s, int pz, int data) {
	if (data == 0) {																							// We have something to do?
		if (pz) {																								// No... we have to put the zero in the buffer?
			*s = '0';																							// Yes...
		}
		
		*(s + pz) = '\0';																						// Add the EOS character
		
		return s;
	}
	
	int i = 511;																								// We don't need to handle negative numbers here...
	
	for (int d = data; d && i; i--, d /= 10) ;																	// Calculate how much bytes we are going to use
	
	i = 511 - i;
	
	s[i--] = '\0';																								// Add the EOS character
	
	for (; data && i >= 0; i--, data /= 10) {																	// And here we go!
		s[i] = "0123456789"[data % 10];
	}
	
	return s;
}

static char *__write_uint(char *restrict s, int pz, int upper, int base, unsigned int data) {
	if (data == 0) {																							// We have something to do?
		if (pz) {																								// No... we have to put the zero in the buffer?
			*s = '0';																							// Yes...
		}
		
		*(s + pz) = '\0';																						// Add the EOS character
		
		return s;
	}
	
	int i = 511;																								// We don't need to handle negative numbers here...
	
	for (unsigned int d = data; d && i; i--, d /= base) ;														// Calculate how much bytes we are going to use
	
	i = 511 - i;
	
	s[i--] = '\0';
	
	for (; data && i >= 0; i--, data /= base) {																	// And here we go!
		s[i] = (upper ? "0123456789ABCDEF" : "0123456789abcdef")[data % base];
	}
	
	return s;
}

int __vprint(int usestream, FILE *restrict stream, int limitn, char *restrict s, size_t n, const char *restrict format, va_list arg) {
	if (usestream && stream == NULL) {																			// First, make sure that the stream is valid
		return EOF;
	} else if (usestream && !(stream->flags & __FLAGS_WRITE)) {													// Make sure that we can write to this file
		return EOF;
	} else if (usestream && (stream->flags & (__FLAGS_EOF | __FLAGS_ERROR | __FLAGS_READING))) {				// Make sure that we hadn't any error/eof, and that we haven't called any read function before
		return EOF;
	}
	
	stream->flags |= __FLAGS_WRITING;																			// Set that we are writing!
	
	int written = 0;
	char buf[512];
	int err = 0;
	
	while (*format) {																							// Let's go!
		if (*format != '%') {																					// Normal character?
			s = __write_char(&err, usestream, stream, limitn, s, n, written, *format++);						// Yes, write it
			
			if (err) {
				return EOF;
			}
			
			written++;
			continue;
		}
		
		const char *restrict old = format++;																	// Save the current pos
		
		va_list olda;																							// Save the current arg pos
		memcpy(&olda, arg, sizeof(va_list));
		va_list_wrapper wrapper;																				// Create the wrapper
		memcpy(&wrapper.arg, arg, sizeof(va_list));
		int lj = 0;																								// This indicates if the conversion should be left-justified (the default is to right-justified)
		int fs = 0;																								// This indicates if we should always append the sign of the conversion (or just a space)
		int alt = 0;																							// This indicates if we should use the alternative form...
		int zero = 0;																							// This indicates if we should use zeros to pad the conversion (default is using spaces to pad)
		int fw = 0;																								// Field width
		int pr = 0;																								// Precision
		int fwset = 0;
		int prset = 0;
		
		format += __parse_flags(format, wrapper, &lj, &fs, &alt, &zero, &fw, &pr, &fwset, &prset);				// Parse the flags (if we have any)
		
		if (!prset) {																							// Set the default precision
			pr = 1;
		}
		
		char padc = prset ? ' ' : (zero ? '0' : ' ');
		char fmtc = *format++;
		
		switch (fmtc) {																							// And here we go! Time to parse the format itself
		case 'd':
		case 'i': {																								// Signed integer
			int val = va_arg(arg, int);																			// Get the value
			char *str = __write_int(buf, pr != 0, val < 0 ? -val : val);										// Convert it into a string
			int len = strlen(str) + (val < 0 || fs);															// Get the full length (so we can pad it correctly)
			int spaces = fw > pr && !zero ? fw - pr : 0;
			int pad = (zero ? (fw > pr ? fw : pr) : pr) - len;
			
			if (fw > len || pr > len) {																			// Pad?
				if (!lj) {																						// Add the spaces now?
					for (int i = 0; i < spaces; i++) {
						s = __write_char(&err, usestream, stream, limitn, s, n, written, padc);
						
						if (err) {
							return EOF;
						}
						
						written++;
					}
				}
				
				if (val < 0) {																					// Put the negative sign?
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '-');						// Yes
					
					if (err) {
						return EOF;
					}
					
					written++;
				} else if (fs == 1) {																			// Put the positive sign?
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '+');						// Yes
					
					if (err) {
						return EOF;
					}
					
					written++;
				} else if (fs == 2) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, ' ');						// Use a space...
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
				
				for (int i = 0; i < pad; i++) {																	// Now, add the final padding...
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '0');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			} else if (val < 0) {																				// Put the negative sign?
				s = __write_char(&err, usestream, stream, limitn, s, n, written, '-');							// Yes
				
				if (err) {
					return EOF;
				}
				
				written++;
			} else if (fs == 1) {																				// Put the positive sign?
				s = __write_char(&err, usestream, stream, limitn, s, n, written, '+');							// Yes
				
				if (err) {
					return EOF;
				}
				
				written++;
			} else if (fs == 2) {
				s = __write_char(&err, usestream, stream, limitn, s, n, written, ' ');							// Use a space...
				
				if (err) {
					return EOF;
				}
				
				written++;
			}
			
			s = __write_string(&err, usestream, stream, limitn, s, n, written, str);							// Finally, write the converted number itself
			
			if (err) {
				return EOF;
			}
			
			written += len - (val < 0 || fs);
			
			if ((fw > len || pr > len) && lj) {																	// Add the spaces now?
				for (int i = 0; i < spaces; i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, padc);
					written++;
				}
			}
			
			break;
		}
		case 'u':
		case 'o':
		case 'x':
		case 'X': {																								// Unsigned integer
			int base = fmtc == 'u' ? 10 : (fmtc == 'o' ? 8 : 16);												// Get the base
			unsigned int val = va_arg(arg, unsigned int);														// Get the value
			char *str = __write_uint(buf, pr != 0, fmtc == 'X', base, val);										// Convert it into a string
			int len = strlen(str) + ((alt && base == 8) ? 1 : ((alt && base == 16) ? 2 : 0));					// Get the full length (so we can pad it correctly)
			int spaces = fw > pr && !zero ? fw - pr : 0;
			int pad = (zero ? (fw > pr ? fw : pr) : pr) - len;
			
			if (fw > len || pr > len) {																			// Pad?
				if (!lj) {																						// Add the spaces now?
					for (int i = 0; i < spaces; i++) {
						s = __write_char(&err, usestream, stream, limitn, s, n, written, padc);
						
						if (err) {
							return EOF;
						}
						
						written++;
					}
				}
				
				if (alt && base == 8) {																			// Put the prefix in the start (octal)?
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '0');						// Yes
					
					if (err) {
						return EOF;
					}
					
					written++;
				} else if (alt && base == 16) {																	// Put the prefix in the start (hex)?
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '0');						// Yes
					
					if (err) {
						return EOF;
					}
					
					s = __write_char(&err, usestream, stream, limitn, s, n, ++written, fmtc);
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
				
				for (int i = 0; i < pad; i++) {																	// Now, add the final padding...
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '0');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			s = __write_string(&err, usestream, stream, limitn, s, n, written, str);							// Finally, write the converted number itself
			
			if (err) {
				return EOF;
			}
			
			written += len - ((alt && base == 8) ? 1 : ((alt && base == 16) ? 2 : 0));
			
			if ((fw > len || pr > len) && lj) {																	// Add the spaces now?
				for (int i = 0; i < spaces; i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, padc);
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			break;
		}
		case 'c': {																								// Single character
			if (!lj) {																							// Pad it now?
				for (int i = 0; i < (fw > 2 ? fw - 1 : 0); i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, ' ');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			s = __write_char(&err, usestream, stream, limitn, s, n, written, (char)va_arg(arg, int));			// And write it
			
			if (err) {
				return EOF;
			}
			
			written++;
			
			if (lj) {																							// Pad it now?
				for (int i = 0; i < (fw > 2 ? fw - 1 : 0); i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, ' ');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			break;
		}
		case 's': {
			char *str = va_arg(arg, char*);																		// Get the string
			int len = strlen(str);																				// The length
			
			if (prset && (int)len > pr) {																		// Limit the size?
				len = pr;																						// Yes, set the new length!
			}
			
			if (!lj) {																							// Pad it now?
				for (int i = 0; i < (fw > len ? fw - len : 0); i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, ' ');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			for (int i = 0; i < len; i++) {
				s = __write_char(&err, usestream, stream, limitn, s, n, written, str[i]);
				
				if (err) {
					return EOF;
				}
				
				written++;
			}
			
			if (lj) {																							// Pad it now?
				for (int i = 0; i < (fw > len ? fw - len : 0); i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, ' ');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			break;
		}
		case 'p': {																								// Pointer
			unsigned int val = va_arg(arg, unsigned int);														// Get the value
			char *str = __write_uint(buf, pr != 0, 0, 16, val);													// Convert it into a string
			int len = strlen(str) + 2;																			// Get the full length (so we can pad it correctly)
			int spaces = fw > pr && !zero ? fw - pr : 0;
			int pad = (zero ? (fw > pr ? fw : pr) : pr) - len;
			
			s = __write_char(&err, usestream, stream, limitn, s, n, written, '0');								// Put the prefix
			
			if (err) {
				return EOF;
			}
			
			s = __write_char(&err, usestream, stream, limitn, s, n, ++written, 'x');
			
			if (err) {
				return EOF;
			}
			
			written++;
			
			if (fw > len || pr > len) {																			// Pad?
				if (!lj) {																						// Add the spaces now?
					for (int i = 0; i < spaces; i++) {
						s = __write_char(&err, usestream, stream, limitn, s, n, written, padc);
						
						if (err) {
							return EOF;
						}
						
						written++;
					}
				}
				
				for (int i = 0; i < pad; i++) {																	// Now, add the final padding...
					s = __write_char(&err, usestream, stream, limitn, s, n, written, '0');
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			s = __write_string(&err, usestream, stream, limitn, s, n, written, str);							// Finally, write the converted number itself
			
			if (err) {
				return EOF;
			}
			
			written += len - 2;
			
			if ((fw > len || pr > len) && lj) {																	// Add the spaces now?
				for (int i = 0; i < spaces; i++) {
					s = __write_char(&err, usestream, stream, limitn, s, n, written, padc);
					
					if (err) {
						return EOF;
					}
					
					written++;
				}
			}
			
			break;
		}
		case 'n': {																								// Just write how many character we have written so far into the user buffer
			*(va_arg(arg, int*)) = written;
			break;
		}
		default: {
			format = old;																						// ... Restore the old pos and the old arg pos
			arg = olda;
			
			__write_char(&err, usestream, stream, limitn, s, n, written, *format++);							// Write the current character (that's going to be a percent sign)
			
			if (err) {
				return written;
			}
			
			written++;			
			break;
		}
		}
	}
	
	return written;
}
