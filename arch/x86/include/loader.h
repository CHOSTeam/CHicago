/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 20 of 2020, at 23:53 BRT
 * Last edited on August 01 of 2020, at 22:06 BRT */

#ifndef __LOADER_H__
#define __LOADER_H__

#include <efi.h>
#include <efilib.h>

/* Boot Information Struct, this is used by the CHicago kernel to get the boot device, get the kernel end,
 * get the framebuffer information, get the initrd, etc... */

#define BOOT_INFO_MAGIC 0xC4057D41
#define BOOT_INFO_DEV_ATA 0x00
#define BOOT_INFO_DEV_SATA 0x01

typedef struct __attribute__((packed)) {
	UINTN Base, Size;
	BOOLEAN Free;
} BOOT_INFO_MEM_MAP;

typedef struct __attribute__((packed)) {
	UINT32 Magic;
	UINTN KernelStart;
	UINT8 *KernelStartVirt;
	UINTN KernelEnd;
	UINT8 *KernelEndVirt;
	UINTN RegionsStart;
	UINT8 *RegionsStartVirt;
	UINTN Directory;
	UINTN EfiMainAddress;
	UINTN MaxPhysicalAddress, PhysicalMemorySize;
	
	struct __attribute__((packed)) {
		UINTN EntryCount;
		BOOT_INFO_MEM_MAP *Entries;
	} MemoryMap;
	
	struct __attribute__((packed)) {
		UINTN Size;
		CHAR8 *Contents;
	} BootSIA;
	
	struct __attribute__((packed)) {
		UINTN Width, Height;
		UINTN Size, Address;
	} FrameBuffer;
	
	struct __attribute__((packed)) {
		UINT8 Type;
		
		struct {
			UINT8 Bus;
			UINT8 Device;
			UINT8 Function;
		} PCI;
		
		union {
			struct {
				BOOLEAN Primary;
				BOOLEAN Master;
			} ATA;
			
			struct {
				UINT16 HBAPort;
				UINT16 Mult;
				UINT8 Lun;
			} SATA;
		};
	} BootDevice;
	
	CHAR8 *KernelStack[0x10000];
} BOOT_INFO;

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
extern EFI_HANDLE *EspHandle;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS InitializeGraphics(VOID);
EFI_STATUS ReadFile(CHAR16 *FileName, CHAR8 **Buffer, UINTN *Size);
EFI_STATUS CreateDirectory(UINTN *DirectoryPhys,
						   UINTN *KernelEndPhys, UINTN *KernelEndVirt,
						   UINTN FrameBufferPhys, UINTN *FrameBufferVirt, UINTN FrameBufferSize,
						   UINTN MaxPhysicalAddress, UINTN *RegionsStartPhys, UINTN *RegionsStartVirt);

#endif
