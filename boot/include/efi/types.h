/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 01 of 2021, at 18:52 BRT
 * Last edited on January 27 of 2021 at 21:21 BRT */

#pragma once

/* Basic/common EFI data types (integers, GUID, handle, etc). */

#define Void void

typedef char Char8;

typedef signed char Int8, Boolean, EfiMacAddress[32], EfiIpv4[4], EfiIpv6[16];
typedef signed short Int16;
typedef signed int Int32;
typedef signed long long Int64;

typedef unsigned char UInt8;
typedef unsigned short UInt16, Char16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64, EfiLba, EfiPhysicalAddress, EfiVirtualAddress;

/* Our -w64-mingw32 compiler defines WIN64/_WIN64 when the dest arch is 64-bits (and we need to know if it is, as the
 * INTN/UINTN size will always be the host's native width). */

#ifdef _WIN64
typedef signed long long IntN;
typedef unsigned long long UIntN, EfiStatus, EfiTpl;
#else
typedef signed int IntN;
typedef unsigned int UIntN, EfiStatus, EfiTpl;
#endif

typedef void *EfiHandle, *EfiEvent;

typedef struct {
    UInt32 Low1;
    UInt16 Low2, Low3;
    UInt8 High[8];
} EfiGuid;

typedef union {
    EfiIpv4 V4;
    EfiIpv6 V6;
} EfiIp;

#define Asm __asm__
#define Volatile __volatile__

#define True 1
#define False 0

#define Null ((Void*)0)

#define INT8_MIN -128
#define INT16_MIN -32768
#define INT32_MIN -2147483648
#define INT64_MIN -9223372036854775808
#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807

#define UINT8_MAX 0xFF
#define UINT16_MAX 0xFFFF
#define UINT32_MAX 0xFFFFFFFF
#define UINT64_MAX 0xFFFFFFFFFFFFFFFF

#ifdef _WIN64
#define INTN_MIN INT64_MIN
#define INTN_MAX INT64_MAX
#define UINTN_MAX UINT64_MAX
#else
#define INTN_MIN INT32_MIN
#define INTN_MAX INT32_MAX
#define UINTN_MAX UINT32_MAX
#endif

/* As the TableHeader struct precedes all the other EFI tables, I think that it is a good idea to define it right now. */

typedef struct {
    UInt64 Signature;
    UInt32 Revision;
    UInt32 HeaderSize;
    UInt32 Crc32;
    UInt32 Reserved;
} EfiTableHeader;

/* And, of course, lots of EFI functions/most EFI functions returns a Status (instead of a Boolean), so let's also define
 * all the valid status codes. */

#ifdef _WIN64
#define EFI_ERROR(x) ((x) & 0x8000000000000000)
#define EFI_CREATE_ERROR(x) (x | 0x8000000000000000)
#else
#define EFI_ERROR(x) ((x) & 0x80000000)
#define EFI_CREATE_ERROR(x) (x | 0x80000000)
#endif

#define EFI_SUCCESS 0x00
#define EFI_WARN_UNKNOWN_GLYPH 0x01
#define EFI_WARN_DELETE_FAILURE 0x02
#define EFI_WARN_WRITE_FAILURE 0x03
#define EFI_WARN_BUFFER_TOO_SMALL 0x04
#define EFI_WARN_STALE_DATA 0x05
#define EFI_WARN_FILE_SYSTEM 0x06
#define EFI_WARN_RESET_REQUIRED 0x07

#define EFI_LOAD_ERROR EFI_CREATE_ERROR(0x01)
#define EFI_INVALID_PARAMETER EFI_CREATE_ERROR(0x02)
#define EFI_UNSUPPORTED EFI_CREATE_ERROR(0x03)
#define EFI_BAD_BUFFER_SIZE EFI_CREATE_ERROR(0x04)
#define EFI_BUFFER_TOO_SMALL EFI_CREATE_ERROR(0x05)
#define EFI_NOT_READY EFI_CREATE_ERROR(0x06)
#define EFI_DEVICE_ERROR EFI_CREATE_ERROR(0x07)
#define EFI_WRITE_PROTECTED EFI_CREATE_ERROR(0x08)
#define EFI_OUT_OF_RESOURCES EFI_CREATE_ERROR(0x09)
#define EFI_VOLUME_CORRUPTED EFI_CREATE_ERROR(0x0A)
#define EFI_VOLUME_FULL EFI_CREATE_ERROR(0x0B)
#define EFI_NO_MEDIA EFI_CREATE_ERROR(0x0C)
#define EFI_MEDIA_CHANGED EFI_CREATE_ERROR(0x0D)
#define EFI_NOT_FOUND EFI_CREATE_ERROR(0x0E)
#define EFI_ACCESS_DENIED EFI_CREATE_ERROR(0x0F)
#define EFI_NO_RESPONSE EFI_CREATE_ERROR(0x10)
#define EFI_NO_MAPPING EFI_CREATE_ERROR(0x11)
#define EFI_TIMEOUT EFI_CREATE_ERROR(0x12)
#define EFI_NOT_STARTED EFI_CREATE_ERROR(0x13)
#define EFI_ALREADY_STARTED EFI_CREATE_ERROR(0x14)
#define EFI_ABORTED EFI_CREATE_ERROR(0x15)
#define EFI_ICMP_ERROR EFI_CREATE_ERROR(0x16)
#define EFI_TFTP_ERROR EFI_CREATE_ERROR(0x17)
#define EFI_PROTOCOL_ERROR EFI_CREATE_ERROR(0x18)
#define EFI_INCOMPATIBLE_VERSION EFI_CREATE_ERROR(0x19)
#define EFI_SECURITY_VIOLATION EFI_CREATE_ERROR(0x1A)
#define EFI_CRC_ERROR EFI_CREATE_ERROR(0x1B)
#define EFI_END_OF_MEDIA EFI_CREATE_ERROR(0x1C)
#define EFI_END_OF_FILE EFI_CREATE_ERROR(0x1F)
#define EFI_INVALID_LANGUAGE EFI_CREATE_ERROR(0x20)
#define EFI_COMPROMISED_DATA EFI_CREATE_ERROR(0x21)
#define EFI_IP_ADDRESS_CONFLICT EFI_CREATE_ERROR(0x22)
#define EFI_HTTP_ERROR EFI_CREATE_ERROR(0x23)
