/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 01 of 2021, at 19:11 BRT
 * Last edited on June 06 of 2021 at 11:23 BRT */

#pragma once

#include <efi/types.h>

#define EFI_RUNTIME_SERVICES_SIGNATURE 0x56524553544E5552
#define EFI_RUNTIME_SERVICES_REV EFI_SPEC_VERSION

/* Time->Daylight values. */

#define EFI_TIME_ADJUST_DAYLIGHT 0x01
#define EFI_TIME_IN_DAYLIGHT 0x02

/* Special Time->Timezone value. */

#define EFI_UNSPECIFIED_TIMEZONE 0x7FF

/* MemoryDescriptor->Attribute values (also the MemoryDescriptor->Version value). */

#define EFI_MEMORY_DESCRIPTOR_VERSION 0x01
#define EFI_MEMORY_UC 0x01
#define EFI_MEMORY_WC 0x02
#define EFI_MEMORY_WT 0x04
#define EFI_MEMORY_WB 0x08
#define EFI_MEMORY_UCE 0x10
#define EFI_MEMORY_WP 0x1000
#define EFI_MEMORY_RP 0x2000
#define EFI_MEMORY_XP 0x4000
#define EFI_MEMORY_NV 0x8000
#define EFI_MEMORY_MORE_RELIABLE 0x10000
#define EFI_MEMORY_RO 0x20000
#define EFI_MEMORY_SP 0x40000
#define EFI_MEMORY_CPU_CRYPTO 0x80000
#define EFI_MEMORY_RUNTIME 0x8000000000000000

/* ConvertPointer first arg value(s). */

#define EFI_OPTIONAL_PTR 0x01

/* GetVariable third argument values (the third argument are the attributes). */

#define EFI_VARIABLE_NON_VOLATILE 0x01
#define EFI_VARIABLE_BOOT_SERVICE_ACCESS 0x02
#define EFI_VARIABLE_RUNTIME_ACCESS 0x04
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 0x08
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS 0x10
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x20
#define EFI_VARIABLE_APPEND_WRITE 0x40
#define EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS 0x80

/* CapsuleHeader->Flags values/bits. */

#define EFI_CAPSULE_FLAGS_PERSIST_ACROSS_RESET 0x10000
#define EFI_CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE 0x20000
#define EFI_CAPSULE_FLAGS_INITIATE_RESET 0x40000

typedef struct {
    UInt16 Year;
    UInt8 Month, Day,
          Hour, Minute, Second;
    UInt8 Pad1;
    UInt32 Nanosecond;
    Int16 Timezone;
    UInt8 Daylight;
    UInt8 Pad2;
} EfiTime;

typedef struct {
    UInt32 Resolution, Accuracy;
    Boolean SetsToZero;
} EfiTimeCapabilities;

typedef struct {
    UInt32 Type;
    EfiPhysicalAddress PhysicalStart;
    EfiVirtualAddress VirtualStart;
    UInt64 NumberOfPages;
    UInt64 Attribute;
} EfiMemoryDescriptor;

typedef enum {
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EfiResetType;

typedef struct {
    UInt64 Length;
    union {
        EfiPhysicalAddress DataBlock, ContinuationPointer;
    } Union;
} EfiCapsuleBlockDescriptor;

typedef struct {
    EfiGuid Guid;
    UInt32 HeaderSize;
    UInt32 Flags;
    UInt32 ImageSize;
} EfiCapsuleHeader;

typedef struct {
    UInt32 ArrayNumber;
    Void *Ptr[1];
} EfiCapsuleTable;

typedef EfiStatus (*_EfiGetTime)(EfiTime*, EfiTimeCapabilities*);
typedef EfiStatus (*_EfiSetTime)(EfiTime*);
typedef EfiStatus (*_EfiGetWakeupTime)(Boolean*, Boolean*, EfiTime*);
typedef EfiStatus (*_EfiSetWakeupTime)(Boolean, EfiTime*);
typedef EfiStatus (*_EfiSetVirtualAddressMap)(UIntN, UIntN, UInt32, EfiMemoryDescriptor*);
typedef EfiStatus (*_EfiConvertPointer)(UIntN, Void**);
typedef EfiStatus (*_EfiGetVariable)(Char16*, EfiGuid*, UInt32*, UIntN*, Void*);
typedef EfiStatus (*_EfiGetNextVariableName)(UIntN*, Char16*, EfiGuid*);
typedef EfiStatus (*_EfiSetVariable)(Char16*, EfiGuid*, UInt32, UIntN, Void*);
typedef EfiStatus (*_EfiGetNextHighMonotonicCount)(UInt32*);
typedef EfiStatus (*_EfiResetSystem)(EfiResetType, EfiStatus, UIntN, Void*);
typedef EfiStatus (*_EfiUpdateCapsule)(EfiCapsuleHeader**, UIntN, EfiPhysicalAddress);
typedef EfiStatus (*_EfiQueryCapsuleCapabilities)(EfiCapsuleHeader**, UIntN, UInt64*, EfiResetType*);
typedef EfiStatus (*_EfiQueryVariableInfo)(UInt32, UInt64*, UInt64*, UInt64*);

typedef struct {
    EfiTableHeader Hdr;

    /* Time services. */

    _EfiGetTime GetTime;
    _EfiSetTime SetTime;
    _EfiGetWakeupTime GetWakeupTime;
    _EfiSetWakeupTime SetWakeupTime;

    /* Virtual memory services. */

    _EfiSetVirtualAddressMap SetVirtualAddressMap;
    _EfiConvertPointer ConvertPointer;

    /* Variable services. */
    
    _EfiGetVariable GetVariable;
    _EfiGetNextVariableName GetNextVariableName;
    _EfiSetVariable SetVariable;
    
    /* Miscellaneous services. */

    _EfiGetNextHighMonotonicCount GetNextHighMonotonicCount;
    _EfiResetSystem ResetSystem;

    /* UEFI 2.0 capsule services. */

    _EfiUpdateCapsule UpdateCapsule;
    _EfiQueryCapsuleCapabilities QueryCapsuleCapabilities;

    /* At last, UEFI 2.0 miscellaneous services. */

    _EfiQueryVariableInfo QueryVariableInfo;
} EfiRuntimeServices;
