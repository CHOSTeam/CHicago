# File author is Ítalo Lima Marconato Matias
#
# Created on November 28 of 2020, at 13:13 BRT
# Last edited on December 04 of 2020, at 13:58 BRT

set(CLANG_KERNEL_TARGET "x86_64-chicago")
set(CLANG_EFI_TARGET "x86_64")

set(KERNEL_BEFORE Kernel/crti.o Kernel/crtbegin.o)
set(KERNEL_AFTER Kernel/crtend.o Kernel/crtn.o Kernel/libgcc.a)
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

function(setup_boot_arch target)
	target_compile_options(${target} PRIVATE $<$<NOT:$<COMPILE_LANGUAGE:ASM-ATT>>:-mcmodel=kernel -mno-red-zone>)
endfunction()

function(setup_kernel_arch target)
	target_compile_options(${target} PRIVATE $<$<NOT:$<COMPILE_LANGUAGE:ASM-ATT>>:-mcmodel=kernel -mno-red-zone>
											 $<$<COMPILE_LANGUAGE:ASM-ATT>:-x assembler-with-cpp -DARCH_64>)
	target_compile_definitions(${target} PRIVATE ARCH_64 ELF_CLASS=0x02 ELF_DATA=0x01 ELF_MACHINE=0x3E)
endfunction()
