/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 31 of 2021, at 13:44 BRT
 * Last edited on February 04 of 2021 at 17:42 BRT */

/* Here we actually need one of the standard compiler headers, to access the offsetof function. */

#include <arch.h>
#include <stddef.h>

__attribute__((noreturn)) Void ArchJumpIntoCHicago(CHBootInfo* BootInfo, UIntN Arg, UIntN Entry, UInt16) {
    if (BootInfo == Null) {
        goto e;
    }

    UInt64 *dir = BootInfo->Directory, sp = (Arg + offsetof(CHBootInfo, KernelStack) +
                                             sizeof(BootInfo->KernelStack)) & -16, val, val2, el;

    /* For setting up the tcr_el1 we need the id_aa64_mmfr0_el1 field (to get the physical address size). Also, let's
     * already disable IRQs and FIQs. */

    Asm Volatile("mrs %0, id_aa64mmfr0_el1\n"
                 "dsb ish; isb; mrs %1, sctlr_el1\n"
                 "mrs %2, CurrentEL\n"
                 "msr daifset, #0x0F; dsb sy; isb sy" : "=r"(val), "=r"(val2), "=r"(el));

    val2 = (val2 | 0xC00800) & ~0x308101F;

    if ((el >> 2) == 1) {
        /* We're already in EL1, so, let's initially disable paging. */

        Asm Volatile("msr sctlr_el1, %0; isb" :: "r"(val2));
    }

    /* Now this section of code is the same for everyone, as it just sets up page translation for EL1. */

    Asm Volatile("msr mair_el1, %0\n"
                 "msr tcr_el1, %1; isb\n"
                 "msr ttbr0_el1, %2\n"
                 "msr ttbr1_el1, %3\n"
                 "msr sctlr_el1, %4; isb\n" :: "r"(0xFFBB4400ull), "r"(0xB5103510ull | ((val & 0x0F) << 32)),
                                               "r"(dir[0] + 1), "r"(dir[1] + 1), "r"(val2 | 0x01));

    /* Now things will change a bit depending on the current EL, as on EL1 we just have to setup the stack, and on
     * EL2 we have to drop to EL1. */

    if ((el >> 2) == 1) {
        Asm Volatile("mov sp, %0" :: "r"(sp));
    } else {
        Asm Volatile("msr sp_el1, %0\n"
                     "msr hcr_el2, %1\n"
                     "msr spsr_el2, %2\n"
                     "adr x4, 1f; msr elr_el2, x4\n"
                     "eret; 1:" :: "r"(sp), "r"(0x80000000ull), "r"(0x3C5ull) : "x4");
    }

    /* Finally, with everything setup correctly (and as we already already under EL1), we can call the kernel
     * entrypoint. */

    ((Void (*)(UIntN))Entry)(Arg);

e:  while (True) {
        Asm Volatile("wfi");
    }
}
