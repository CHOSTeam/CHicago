# File author is Ítalo Lima Marconato Matias
#
# Created on November 29 of 2020, at 15:43 BRT
# Last edited on December 04 of 2020, at 13:59 BRT

cmake_minimum_required(VERSION 3.12.0)

# We need to set the module path before calling project(), as we have a custom platform file.

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../host-tools/cmake")
else()
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/host-tools/cmake")
endif()

project(boot C ASM-ATT)

include(CHOSOptions)
include(${CHOS_ARCH}Specific)

set(BOOT_SOURCES arch/vmm${CHOS_ARCH}.c file.c gop.c main.c sia.c)
list(TRANSFORM BOOT_SOURCES PREPEND ${PROJECT_SOURCE_DIR}/)

add_executable(osldr.efi ${BOOT_SOURCES})

# Some info about the target_* stuff: we expect that clang is the compiler (so we use -target and -fuse-ld=lld),
# we need to manually set the entry point, and remember to set the subsystem (as this is no normal EXE, this is
# an EFI application). Also, we need to set base, as the default base (at least on x86) is way too close to the
# kernel load location (on x86, the kernel gets loaded at 1MiB, and the default load address for the application
# is at 4MiB).

target_include_directories(osldr.efi PRIVATE ${PROJECT_SOURCE_DIR}/include
											 ${CHOS_TOOLCHAIN}/efi/include
											 ${CHOS_TOOLCHAIN}/efi/include/protocol
											 ${CHOS_TOOLCHAIN}/efi/include/${CHOS_ARCH})
target_compile_definitions(osldr.efi PRIVATE VERSION=L\"${CHOS_VERSION}\")
target_compile_options(osldr.efi PRIVATE -target ${CLANG_EFI_TARGET}-unknown-windows-gnu)
target_compile_options(osldr.efi PRIVATE $<$<NOT:$<COMPILE_LANGUAGE:ASM-ATT>>:-fshort-wchar -ffreestanding
																			  -fno-strict-aliasing
																			  -fno-stack-protector
																			  -fno-stack-check>)
target_link_options(osldr.efi PRIVATE -target ${CLANG_EFI_TARGET}-unknown-windows-gnu -fuse-ld=lld -nostdlib
									  -Wl,/entry:EfiMain -Wl,/subsystem:efi_application -Wl,/base:0x10000000)
target_link_libraries(osldr.efi ${CHOS_TOOLCHAIN}/efi/lib/${CHOS_ARCH}/efi.lib
								${CHOS_TOOLCHAIN}/efi/lib/${CHOS_ARCH}/sup.lib)
setup_boot_arch(osldr.efi)								
