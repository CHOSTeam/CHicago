/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 31 of 2021, at 13:45 BRT
 * Last edited on February 05 of 2021 at 17:44 BRT */

#include <arch.h>
#include <stddef.h>

__attribute__((noreturn)) Void ArchJumpIntoCHicago(CHBootInfo *BootInfo, UIntN Arg, UIntN Entry, UInt16) {
    if (BootInfo == Null) {
        goto e;
    }

    /* Initialize the FPU/SSE support. */

    UIntN sp = (Arg + offsetof(CHBootInfo, KernelStack) + sizeof(BootInfo->KernelStack)) & -16, val1, val2;
    UInt16 cw0 = 0x37E, cw1 = 0x37A;

    Asm Volatile("mov %%cr0, %0\n"
                 "mov %%cr4, %1" : "=r"(val1), "=r"(val2));
    Asm Volatile("mov %0, %%cr0\n"
                 "mov %1, %%cr4\n"
                 "fninit\n"
                 "fldcw %2\n"
                 "fldcw %3\n"
                 "cli" :: "r"((val1 & ~0x04) | 0x02), "r"(val2 | 0x600), "m"(cw0), "m"(cw1));

    /* Disable interrupts/exceptions/IRQs, enable some required things (like large/huge pages), set the new pagedir,
     * and jump to the kernel!
     * One little thing: On amd64, we have to remember to set the NXE bit on the EFER, else, we may crash on systems that
     * the firmware doesn't enable it by default. */

#ifdef __i386__
    Asm Volatile("mov %0, %%ebx\n"
                 "mov %1, %%ecx\n"
                 "mov %2, %%edx\n"
                 "mov %3, %%cr3\n"
                 "mov %%cr4, %%eax; or $0x10, %%eax; mov %%eax, %%cr4\n"
                 "mov %%cr0, %%eax; or $0x80010001, %%eax; mov %%eax, %%cr0\n"
                 "mov %%edx, %%esp\n"
                 "push %%ecx\n"
                 "call *%%ebx" :: "r"(Entry), "r"(Arg), "r"(sp), "r"(BootInfo->Directory) : "%ebx", "%ecx", "%edx");
#else
    Asm Volatile("rdmsr" : "=r"(val1) : "c"(0xC0000080));
    Asm Volatile("wrmsr" :: "r"(val1 | 0x800), "c"(0xC0000080));
    Asm Volatile("mov %0, %%rbx\n"
                 "mov %1, %%rdi\n"
                 "mov %2, %%rsi\n"
                 "mov %3, %%cr3\n"
                 "mov %%rsi, %%rsp\n"
                 "call *%%rbx" :: "r"(Entry), "r"(Arg), "r"(sp), "r"(BootInfo->Directory) : "%rbx", "%rdi", "%rsi");
#endif

e:  while (True) {
        Asm Volatile("hlt");
    }
}
