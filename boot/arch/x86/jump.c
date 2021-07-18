/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 31 of 2021, at 13:45 BRT
 * Last edited on July 18 of 2021 at 18:43 BRT */

#include <arch.h>
#include <stddef.h>

__attribute__((noreturn)) Void ArchJumpIntoCHicago(CHBootInfo *BootInfo, UIntN Arg, UIntN Entry, UInt16) {
    if (BootInfo == Null) goto e;

    /* Initialize the FPU (SSE+AVX2) support. */

    UIntN sp = (Arg + offsetof(CHBootInfo, KernelStack) + sizeof(BootInfo->KernelStack)) & -16, val1, val2;
    UInt16 cw0 = 0x37E, cw1 = 0x37A;

    asm volatile("mov %%cr0, %0; mov %%cr4, %1" : "=r"(val1), "=r"(val2));
    asm volatile("mov %0, %%cr0; mov %1, %%cr4\n"
                 "fninit; fldcw %2; fldcw %3\n"
#ifdef __i386__
                 "xor %%ecx, %%ecx; xgetbv; or $0x07, %%eax; xsetbv\n"
#else
                 "xor %%rcx, %%rcx; xgetbv; or $0x07, %%rax; xsetbv\n"
#endif
                 "cli" :: "r"((val1 & ~0x04) | 0x02), "r"(val2 | 0x40600), "m"(cw0), "m"(cw1)
#ifdef __i386__
                        : "%eax", "%ecx", "%edx");
#else
                        : "%rax", "%rcx", "%rdx");
#endif

    /* Disable interrupts/exceptions/IRQs, enable some required things (like large/huge pages), set the new pagedir,
     * and jump to the kernel!
     * Also on x86, we have to take caution with the stack alignment, as we have one push instruction before the call
     * (and the System V ABI expects the stack to be 16-bytes aligned before the call). */

    asm volatile("rdmsr" : "=a"(val1) : "c"(0xC0000080));
    asm volatile("wrmsr" :: "A"(val1 | 0x800), "c"(0xC0000080));

#ifdef __i386__
    if (sp + 4 > Arg + offsetof(CHBootInfo, KernelStack) + sizeof(BootInfo->KernelStack)) sp -= 0x10;

    asm volatile("mov %%cr0, %%eax; btr $31, %%eax; mov %%eax, %%cr0" ::: "%eax");
    asm volatile("mov %0, %%ebx; mov %1, %%ecx; mov %2, %%edx; mov %3, %%cr3\n"
                 "mov $0x40778, %%eax; mov %%eax, %%cr4; mov %%cr0, %%eax; or $0x80010001, %%eax; mov %%eax, %%cr0\n"
                 "mov %%edx, %%esp; xor %%ebp, %%ebp; push %%ecx; call *%%ebx"
                 :: "r"(Entry), "r"(Arg), "r"(sp + 4), "r"(BootInfo->Directory) : "%ebx", "%ecx", "%edx");
#else
    asm volatile("mov %0, %%rbx; mov %1, %%rdi; mov %2, %%rsi; mov %3, %%cr3\n"
                 "mov $0x40778, %%rax; mov %%rax, %%cr4\n"
                 "mov %%rsi, %%rsp; xor %%rbp, %%rbp; call *%%rbx"
                 :: "r"(Entry), "r"(Arg), "r"(sp), "r"(BootInfo->Directory) : "%rbx", "%rdi", "%rsi");
#endif

e:  while (True) asm volatile("hlt");
}
