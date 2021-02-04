/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 02 of 2021, at 15:09 BRT
 * Last edited on January 31 of 2021 at 12:02 BRT */

#pragma once

#include <efi/dp.h>
#include <efi/rs.h>

#define EFI_BOOT_SERVICES_SIGNATURE 0x56524553544F4F42
#define EFI_BOOT_SERVICES_REV EFI_SPEC_VERSION

/* Task priority levels. */

#define EFI_TPL_APPLICATION 0x04
#define EFI_TPL_CALLBACK 0x08
#define EFI_TPL_NOTIFY 0x10
#define EFI_TPL_HIGH_LEVEL 0x1F

/* Event types. */

#define EFI_EVT_TIMER 0x80000000
#define EFI_EVT_RUNTIME 0x40000000
#define EFI_EVT_NOTIFY_WAIT 0x100
#define EFI_EVT_NOTIFY_SIGNAL 0x200
#define EFI_EVT_SIGNAL_EXIT_BOOT_SERVICES 0x201
#define EFI_EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

/* OpenProtocol attributes/flags. */

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x01
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x02
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x04
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x08
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x10
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x20

typedef enum {
    EfiAllocateAnyPages,
    EfiAllocateMaxAddress,
    EfiAllocateAddress,
    EfiMaxAllocateType
} EfiAllocateType;

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistantMemory,
    EfiMaxMemoryType
} EfiMemoryType;

typedef enum {
    EfiTimerCancel,
    EfiTimerPeriodic,
    EfiTimerRelative
} EfiTimerDelay;

typedef enum {
    EfiNativeInterface
} EfiInterfaceType;

typedef enum {
    EfiAllHandles,
    EfiByRegisterNotify,
    EfiByProtocol
} EfiLocateSearchType;

typedef struct {
    EfiHandle AgentHandle;
    EfiHandle ControllerHandle;
    UInt32 Attributes;
    UInt32 OpenCount;
} EfiOpenProtocolInformationEntry;

typedef EfiTpl (*_EfiRaiseTpl)(EfiTpl);
typedef Void (*_EfiRestoreTpl)(EfiTpl);
typedef EfiStatus (*_EfiAllocatePages)(EfiAllocateType, EfiMemoryType, UIntN, EfiPhysicalAddress*);
typedef EfiStatus (*_EfiFreePages)(EfiPhysicalAddress, UIntN);
typedef EfiStatus (*_EfiGetMemoryMap)(UIntN*, EfiMemoryDescriptor*, UIntN*, UIntN*, UInt32*);
typedef EfiStatus (*_EfiAllocatePool)(EfiMemoryType, UIntN, Void**);
typedef EfiStatus (*_EfiFreePool)(Void*);
typedef EfiStatus (*_EfiEventNotify)(EfiEvent, Void*);
typedef EfiStatus (*_EfiCreateEvent)(UInt32, EfiTpl, _EfiEventNotify, Void*, EfiEvent*);
typedef EfiStatus (*_EfiSetTimer)(EfiEvent, EfiTimerDelay, UInt64);
typedef EfiStatus (*_EfiWaitForEvent)(UIntN, EfiEvent*, UIntN*);
typedef EfiStatus (*_EfiSignalEvent)(EfiEvent);
typedef EfiStatus (*_EfiCloseEvent)(EfiEvent);
typedef EfiStatus (*_EfiCheckEvent)(EfiEvent);
typedef EfiStatus (*_EfiInstallProtocolInterface)(EfiHandle*, EfiGuid*, EfiInterfaceType, Void*);
typedef EfiStatus (*_EfiReinstallProtocolInterface)(EfiHandle, EfiGuid*, Void*, Void*);
typedef EfiStatus (*_EfiUninstallProtocolInterface)(EfiHandle, EfiGuid*, Void*);
typedef EfiStatus (*_EfiHandleProtocol)(EfiHandle, EfiGuid*, Void**);
typedef EfiStatus (*_EfiRegisterProtocolNotify)(EfiGuid*, EfiEvent, Void**);
typedef EfiStatus (*_EfiLocateHandle)(EfiLocateSearchType, EfiGuid*, Void*, UIntN*, EfiHandle*);
typedef EfiStatus (*_EfiLocateDevicePath)(EfiGuid*, EfiDevicePath**, EfiHandle*);
typedef EfiStatus (*_EfiInstallConfigurationTable)(EfiGuid*, Void*);
typedef EfiStatus (*_EfiImageLoad)(Boolean, EfiHandle, EfiDevicePath*, Void*, UIntN, EfiHandle*);
typedef EfiStatus (*_EfiImageStart)(EfiHandle, UIntN*, Char16**);
typedef EfiStatus (*_EfiImageExit)(EfiHandle, EfiStatus, UIntN, Char16*);
typedef EfiStatus (*_EfiImageUnload)(EfiHandle);
typedef EfiStatus (*_EfiExitBootServices)(EfiHandle, UIntN);
typedef EfiStatus (*_EfiGetNextMonotonicCount)(UInt64*);
typedef EfiStatus (*_EfiStall)(UIntN);
typedef EfiStatus (*_EfiSetWatchdogTimer)(UIntN, UInt64, UIntN, Char16*);
typedef EfiStatus (*_EfiConnectController)(EfiHandle, EfiHandle*, EfiDevicePath*, Boolean);
typedef EfiStatus (*_EfiDisconnectController)(EfiHandle, EfiHandle, EfiHandle);
typedef EfiStatus (*_EfiOpenProtocol)(EfiHandle, EfiGuid*, Void**, EfiHandle, EfiHandle, UInt32);
typedef EfiStatus (*_EfiCloseProtocol)(EfiHandle, EfiGuid*, EfiHandle, EfiHandle);
typedef EfiStatus (*_EfiOpenProtocolInformation)(EfiHandle, EfiGuid*, EfiOpenProtocolInformationEntry**, UIntN*);
typedef EfiStatus (*_EfiProtocolsPerHandle)(EfiHandle, EfiGuid***, UIntN*);
typedef EfiStatus (*_EfiLocateHandleBuffer)(EfiLocateSearchType, EfiGuid*, Void*, UIntN*, EfiHandle**);
typedef EfiStatus (*_EfiLocateProtocol)(EfiGuid*, Void*, Void**);
typedef EfiStatus (*_EfiInstallMultipleProtocolInterfaces)(EfiHandle*, ...);
typedef EfiStatus (*_EfiUninstallMultipleProtocolInterfaces)(EfiHandle, ...);
typedef EfiStatus (*_EfiCalculateCrc32)(Void*, UIntN, UInt32*);
typedef EfiStatus (*_EfiCopyMem)(Void*, Void*, UIntN);
typedef EfiStatus (*_EfiSetMem)(Void*, UIntN, UInt8);
typedef EfiStatus (*_EfiCreateEventEx)(UInt32, EfiTpl, _EfiEventNotify, const Void*, const EfiGuid*, EfiEvent*);

typedef struct {
    EfiTableHeader Hdr;

    /* Task priority/TPL services. */

    _EfiRaiseTpl RaiseTpl;
    _EfiRestoreTpl RestoreTpl;

    /* Memory services. */

    _EfiAllocatePages AllocatePages;
    _EfiFreePages FreePages;
    _EfiGetMemoryMap GetMemoryMap;
    _EfiAllocatePool AllocatePool;
    _EfiFreePool FreePool;

    /* Event (and timer) services. */

    _EfiCreateEvent CreateEvent;
    _EfiSetTime SetTimer;
    _EfiWaitForEvent WaitForEvent;
    _EfiSignalEvent SignalEvent;
    _EfiCloseEvent CloseEvent;
    _EfiCheckEvent CheckEvent;

    /* Protocol handling services. */

    _EfiInstallProtocolInterface InstallProtocolInterface;
    _EfiReinstallProtocolInterface ReinstallProtocolInterface;
    _EfiUninstallProtocolInterface UninstallProtocolInterface;
    _EfiHandleProtocol HandleProtocol;
    Void *Reserved;
    _EfiRegisterProtocolNotify RegisterProtocolNotify;
    _EfiLocateHandle LocateHandle;
    _EfiLocateDevicePath LocateDevicePath;
    _EfiInstallConfigurationTable InstallConfigurationTable;

    /* Image services. */

    _EfiImageLoad LoadImage;
    _EfiImageStart StartImage;
    _EfiImageExit Exit;
    _EfiImageUnload UnloadImage;
    _EfiExitBootServices ExitBootServices;

    /* Miscellanous services. */

    _EfiGetNextMonotonicCount GetNextMonotonicCount;
    _EfiStall Stall;
    _EfiSetWatchdogTimer SetWatchdogTimer;

    /* Driver support services. */

    _EfiConnectController ConnectController;
    _EfiDisconnectController DisconnectController;

    /* Open/close protocol services. */

    _EfiOpenProtocol OpenProtocol;
    _EfiCloseProtocol CloseProtocol;
    _EfiOpenProtocolInformation OpenProtocolInformation;

    /* Library services. */
    
    _EfiProtocolsPerHandle ProtocolsPerHandle;
    _EfiLocateHandleBuffer LocateHandleBuffer;
    _EfiLocateProtocol LocateProtocol;
    _EfiInstallMultipleProtocolInterfaces InstallMultipleProtocolInterfaces;
    _EfiUninstallMultipleProtocolInterfaces UninstallMultipleProtocolInterfaces;

    /* 32-bit CRC services. */

    _EfiCalculateCrc32 CalculateCrc32;

    /* Miscellaneous services (again). */

    _EfiCopyMem CopyMem;
    _EfiSetMem SetMem;
    _EfiCreateEventEx CreateEventEx;
} EfiBootServices;
