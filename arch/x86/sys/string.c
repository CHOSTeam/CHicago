// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 19:05 BRT
// Last edited on September 01 of 2019, at 15:27 BRT

#include <chicago/alloc.h>

PVoid StrCopyMemory(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt8)dest)[i] = ((PUInt8)src)[i];
	}
	
	return dest;
}

PVoid StrCopyMemory16(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt16)dest)[i] = ((PUInt16)src)[i];
	}
	
	return dest;
}

PVoid StrSetMemory(PVoid dest, UInt8 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt8)dest)[i] = val;
	}
	
	return dest;
}

PVoid StrSetMemory16(PVoid dest, UInt16 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt16)dest)[i] = val;
	}
	
	return dest;
}

Boolean StrCompareMemory(PVoid m1, PVoid m2, UIntPtr count) {
	if ((m1 == Null) || (m2 == Null) || (count == 0)) {								// m1 is an Null pointer? m2 is an Null pointer? Zero-sized compare?
		return False;																// Yes
	}
	
	PUInt8 mp1 = m1;
	PUInt8 mp2 = m2;
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		if (*mp1++ != *mp2++) {
			return False;
		}
	}
	
	return True;
}

UIntPtr StrGetLength(PChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;
	
	for (; str[n] != 0; n++) ;														// Again, GCC should optimize this for us (if possible)
	
	return n;
}

UIntPtr StrGetLengthW(PWChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;
	
	for (; str[n] != 0; n++) ;														// Again, GCC should optimize this for us (if possible)
	
	return n;
}

Boolean StrCompare(PChar dest, PChar src) {
	return StrCompareMemory(dest, src, StrGetLength(dest));							// Just redirect to the StrCompareMemory function
}

Boolean StrCompareW(PWChar dest, PWChar src) {
	return StrCompareMemory(dest, src, StrGetLengthW(dest) * 2);					// Just redirect to the StrCompareMemory function
}

PChar StrCopy(PChar dest, PChar src) {
	return StrCopyMemory(dest, src, StrGetLength(src) + 1);							// Just redirect to StrCopyMemory function
}

Void StrConcatenate(PChar dest, PChar src) {
	if ((dest == Null) || (src == Null)) {											// Destination is an Null pointer? Source is an Null pointer?
		return;																		// Yes :(
	}
	
	PChar end = dest + StrGetLength(dest);											// Let's do it!
	
	StrCopyMemory(end, src, StrGetLength(src));										// Let's append the src to dest
	
	end += StrGetLength(src);														// And put an 0 (NUL) at the end
	*end = '\0';
}

PChar StrDuplicate(PChar str) {
	PChar ret = (PChar)MemAllocate(StrGetLength(str) + 1);
	
	if (ret == Null) {
		return Null;
	}
	
	return StrCopy(ret, str);
}

static Void StrFormatWriteCharacter(PChar str, UIntPtr n, Char data) {
	if (str != Null) {																// We have our string?
		str[n] = data;																// Yes, write to it!
	}
}

static UIntPtr StrFormatWriteString(PChar str, UIntPtr n, PChar data) {
	UIntPtr len = StrGetLength(data);												// Get the length of the data
	
	if (str != Null) {																// We have our string?
		StrCopyMemory(&str[n], data, len);											// Yes, write the string to it!
	}
	
	return len;																		// Return the length of the string
}

static UIntPtr StrFormatWriteInteger(PChar str, UIntPtr n, UIntPtr data, UInt8 base) {
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
	
	for (len = 30 - i, i = 30 - i - 1; data && i >= 0; i--, data /= base) {			// Yes we have it, so let's write!
		str[i] = L"0123456789ABCDEF"[data % base];
	}
	
	return len;
}

UIntPtr StrFormat(PChar str, PChar data, ...) {
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
				n += StrFormatWriteString(str, n, (PChar)VariadicArg(va, PChar));
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
	
	return n + 1;
}

PChar StrTokenize(PChar str, PChar delim) {
	static PChar temp = Null;
	
	if (str != Null) {																// First call?
		if (temp != Null) {															// Free the current temp?
			MemFree((UIntPtr)temp);													// Yup
		}
		
		temp = StrDuplicate(str);													// Yes, try to set the copy the str to temp
		
		if (temp == Null) {															// Failed?
			return Null;															// Yes
		}
	} else if (temp == Null) {														// Not the first call but temp is Null?
		return Null;																// Yes, so return Null
	} else {
		str = temp;
	}
	
	UIntPtr chars = 0;
	UIntPtr flag = 0;
	
	while (*temp) {
		for (PChar d = delim; *d != '\0'; d++) {
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
	
	MemFree((UIntPtr)temp);
	
	return str;
}
