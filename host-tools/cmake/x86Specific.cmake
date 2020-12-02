# File author is Ítalo Lima Marconato Matias
#
# Created on November 28 of 2020, at 11:05 BRT
# Last edited on November 30 of 2020, at 16:50 BRT

set(CLANG_KERNEL_TARGET "i686-chicago")
set(CLANG_EFI_TARGET "i686")

set(KERNEL_BEFORE Kernel/crti.o crtbegin.o)
set(KERNEL_AFTER crtend.o Kernel/crtn.o libgcc.a)
list(TRANSFORM KERNEL_BEFORE PREPEND ${CHOS_SYSROOT}/Development/Libraries/)
list(TRANSFORM KERNEL_AFTER PREPEND ${CHOS_SYSROOT}/Development/Libraries/)
string(REPLACE ";" " " KERNEL_BEFORE "${KERNEL_BEFORE}")
string(REPLACE ";" " " KERNEL_AFTER "${KERNEL_AFTER}")

set(KERNEL_ARCH_SOURCES misc/debug.cxx
						
						mm/pmm.cxx
						mm/vmm.cxx
						
						sys/arch.cxx
						sys/entry.cxx
						sys/gdt.cxx
						sys/idt.cxx
						sys/idt.s
						sys/process.cxx
						sys/timer.cxx)
list(TRANSFORM KERNEL_ARCH_SOURCES PREPEND arch/x86/)

function(setup_kernel_arch target)
	target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:ASM-ATT>:-x assembler-with-cpp>)
	target_compile_definitions(${target} PRIVATE ELF_CLASS=0x01 ELF_DATA=0x01 ELF_MACHINE=0x03)
endfunction()
