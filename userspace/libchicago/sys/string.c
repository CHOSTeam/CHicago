// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 19:05 BRT
// Last edited on November 17 of 2019, at 09:56 BRT

#include <chicago/alloc.h>

PVoid StrCopyMemory(PVoid restrict dest, PVoid restrict src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	PUInt8 sr = src;
	PUInt8 dst = dest;
	
	while (count--) {																// GCC should optimize this for us :)
		*dst++ = *sr++;
	}
	
	return dest;
}

PVoid StrCopyMemory24(PVoid restrict dest, PVoid restrict src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	PUInt8 src8 = src;
	PUInt8 dst8 = dest;
	PUInt16 src16 = (PUInt16)(src8 + 1);
	PUInt16 dst16 = (PUInt16)(dst8 + 1);
	
	while (count--) {																// GCC should optimize this for us :)
		*dst8 = *src8;
		*dst16++ = *src16++;
		dst8 += 3;
		src8 += 3;
	}
	
	return dest;
}

PVoid StrCopyMemory32(PVoid restrict dest, PVoid restrict src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	PUInt32 sr = src;
	PUInt32 dst = dest;
	
	while (count--) {																// GCC should optimize this for us :)
		*dst++ = *sr++;
	}
	
	return dest;
}

PVoid StrSetMemory(PVoid dest, UInt8 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	PUInt8 dst = dest;
	
	while (count--) {																// GCC should optimize this for us :)
		*dst++ = val;
	}
	
	return dest;
}

PVoid StrSetMemory24(PVoid dest, UInt32 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	PUInt8 dst8 = dest;
	PUInt16 dst16 = (PUInt16)(dst8 + 1);
	UInt16 val16 = (UInt16)val;
	UInt8 val8 = (UInt8)(val << 16);
	
	while (count--) {																// GCC should optimize this for us :)
		*dst8 = val8;
		*dst16++ = val16;
		dst8 += 3;
	}
	
	return dest;
}

PVoid StrSetMemory32(PVoid dest, UInt32 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	PUInt32 dst = dest;
	
	while (count--) {																// GCC should optimize this for us :)
		*dst++ = val;
	}
	
	return dest;
}

Boolean StrCompareMemory(PVoid restrict m1, PVoid restrict m2, UIntPtr count) {
	if ((m1 == Null) || (m2 == Null) || (count == 0)) {								// m1 is an Null pointer? m2 is an Null pointer? Zero-sized compare?
		return False;																// Yes
	}
	
	PUInt8 mp1 = m1;
	PUInt8 mp2 = m2;
	
	while (count--) {																// GCC should optimize this for us :)
		if (*mp1++ != *mp2++) {
			return False;
		}
	}
	
	return True;
}

UIntPtr StrGetLength(PWChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;
	
	for (; str[n] != 0; n++) ;														// Again, GCC should optimize this for us (if possible)
	
	return n;
}

Boolean StrCompare(PWChar dest, PWChar src) {
	return StrCompareMemory(dest, src, StrGetLength(dest) * sizeof(WChar));			// Just redirect to the StrCompareMemory function
}

PWChar StrCopy(PWChar dest, PWChar src) {
	return StrCopyMemory(dest, src, (StrGetLength(src) + 1) * sizeof(WChar));		// Just redirect to StrCopyMemory function
}

Void StrConcatenate(PWChar dest, PWChar src) {
	if ((dest == Null) || (src == Null)) {											// Destination is an Null pointer? Source is an Null pointer?
		return;																		// Yes :(
	}
	
	PWChar end = dest + StrGetLength(dest);											// Let's do it!
	
	StrCopyMemory(end, src, StrGetLength(src) * sizeof(WChar));						// Let's append the src to dest
	
	end += StrGetLength(src);														// And put an 0 (NUL) at the end
	*end = '\0';
}

PWChar StrDuplicate(PWChar str) {
	PWChar ret = (PWChar)MmAllocMemory((StrGetLength(str) + 1) * sizeof(WChar));
	
	if (ret == Null) {
		return Null;
	}
	
	return StrCopy(ret, str);
}

static Void StrFormatWriteCharacter(PWChar str, UIntPtr n, WChar data) {
	if (str != Null) {																// We have our string?
		str[n] = data;																// Yes, write to it!
	}
}

static UIntPtr StrFormatWriteString(PWChar str, UIntPtr n, PWChar data) {
	UIntPtr len = StrGetLength(data);												// Get the length of the data
	
	if (str != Null) {																// We have our string?
		StrCopyMemory(&str[n], data, len * sizeof(WChar));							// Yes, write the string to it!
	}
	
	return len;																		// Return the length of the string
}

static UIntPtr StrFormatWriteInteger(PWChar str, UIntPtr n, UIntPtr data, UInt8 base) {
	if (data == 0) {																// Our algorithm doesn't works if the data is 0
		if (str != Null) {															// We have our string?
			str[n] = '0';															// Yes, write to it!
		}
		
		return 1;
	}
	
	UIntPtr d = data;
	IntPtr len = 0;
	Int i = 30;																		// Let's get the size of the dest string
	
	for (; d && i; i--, d /= base) ;
	
	if (str == Null) {																// We have our string?
		return 30 - i;																// Nope, just return the length
	}
	
	for (len = 30 - i, i = n + 30 - i - 1; data && i >= 0; i--, data /= base) {		// Yes we have it, so let's write!
		str[i] = L"0123456789ABCDEF"[data % base];
	}
	
	return len;
}

UIntPtr StrFormat(PWChar str, PWChar data, ...) {
	if (data == Null) {																// We have any string to format?
		return 0;																	// Nope :(
	}
	
	UIntPtr n = 0;
	VariadicList va;
	
	VariadicStart(va, data);														// Let's start our va list with the arguments provided by the user (if any)
	
	for (UIntPtr i = 0; i < StrGetLength(data); i++) {
		if (data[i] != '%') {														// It's an % (integer, string, character or other)?
			StrFormatWriteCharacter(str, n++, data[i]);								// Nope	
		} else {
			switch (data[++i]) {													// Yes, let's parse it!
			case 's': {																// String
				n += StrFormatWriteString(str, n, (PWChar)VariadicArg(va, PWChar));
				break;
			}
			case 'c': {																// Character
				StrFormatWriteCharacter(str, n++, (Char)VariadicArg(va, Int));
				break;
			}
			case 'd': {																// Decimal Number
				n += StrFormatWriteInteger(str, n, (UIntPtr)VariadicArg(va, UIntPtr), 10);
				break;
			}
			case 'x': {																// Hexadecimal Number
				n += StrFormatWriteInteger(str, n, (UIntPtr)VariadicArg(va, UIntPtr), 16);
				break;
			}
			default: {																// Probably it's another % (probably)
				StrFormatWriteCharacter(str, n++, data[i]);
				break;
			}
			}
		}
	}
	
	if (str != Null) {																// End the str?
		str[n] = '\0';																// Yes
	}
	
	VariadicEnd(va);
	
	return (n + 1) * sizeof(WChar);
}

PWChar StrTokenize(PWChar str, PWChar delim) {
	static PWChar temp = Null;
	
	if (str != Null) {																// First call?
		if (temp != Null) {															// Free the current temp?
			MmFreeMemory((UIntPtr)temp);											// Yup
		}
		
		temp = StrDuplicate(str);													// Yes, try to set the copy the str to temp
		
		if (temp == Null) {															// Failed?
			return Null;															// Yes
		}
	} else if (temp == Null) {														// Not the first call but temp is Null?
		return Null;																// Yes, so return Null
	}
	
	str = temp;																		// Set the string
	
	UIntPtr chars = 0;
	UIntPtr flag = 0;
	
	while (*temp) {
		for (PWChar d = delim; *d != '\0'; d++) {
			if (*temp == *d) {														// Found delim in the string?
				if (chars == 0) {													// First character in the string?
					flag = 1;														// Yes, there may be other after it, so go to the next one
					str++;
				} else {
					temp++;															// No, so we can return
					str[chars] = '\0';
					
					return str;
				}
			}
		}
		
		if (flag == 0) {															// Found delim?
			chars++;																// No, so increase chars
		}
		
		temp++;
		flag = 0;
	}
	
	temp = Null;
	str[chars] = '\0';
	str = StrDuplicate(str);														// *HACKHACKHACK*
	
	MmFreeMemory((UIntPtr)temp);
	
	return str;
}
