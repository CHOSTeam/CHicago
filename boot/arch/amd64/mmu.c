/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 28 of 2021, at 09:16 BRT
 * Last edited on January 31 of 2021 at 21:46 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static UIntN MmuHigherHalfStart = 0xFFFF800000000000, MmuHigherHalfEnd = 0xFFFFFF0000000000;
static Boolean MmuSupports57 = False;

static Void MmuMap2MB(UInt64 *PD, UIntN Virtual, UIntN Physical, Boolean NoExec) {
    /* Same as ARM64, we have to take out the first 21 bits and truncate the value into 0-511. */

    PD[(Virtual >> 21) & 0x1FF] = (Physical & ~0x1FFFFF) | MMU_PRESENT | MMU_HUGE | (NoExec * MMU_NO_EXEC);
}

static EfiStatus MmuWalkLevel(UInt64 *Level, UIntN *LastPhys, UIntN Virtual, UInt8 Shift, UIntN *Out) {
    UInt64 tbl = Level[(Virtual >> Shift) & 0x1FF];

    if (!(tbl & MMU_PRESENT)) {
        EfiZeroMemory((Void*)*LastPhys, 0x1000);
        Level[(Virtual >> Shift) & 0x1FF] = (*LastPhys & 0xFFF) | MMU_PRESENT;
        tbl = Level[(Virtual >> Shift) & 0x1FF];
        *LastPhys += 0x1000;
    } else if (tbl & MMU_HUGE) {
        EfiDrawString("The MMU paging structures got corrupted during the initialization process.",
                      5, EfiFont.Height + 15, 0xFF, 0xFF, 0xFF);
        return EFI_UNSUPPORTED;
    }

    *Out = tbl & ~0xFFF;

    return EFI_SUCCESS;
} 

static EfiStatus MmuMap(UInt64 *PageDir, UIntN *LastPhys, UIntN Virtual, UIntN Physical, Boolean NoExec) {
    UIntN shift = MmuSupports57 ? 48 : 39;

    /* Let's already align the addresses to 2MB. */

    Virtual &= ~0x1FFFFF;
    Physical &= ~0x1FFFFF;

    /* On both 57 and 48-bits addressing we need to walk through the first/last two levels (PML5 and PML4 on 57-bits,
     * PML4 and PDP on 48-bits), but on 57-bits we need to walk one extra level, as it has one extra level before the
     * PDP (well, it's actually before the PML4). */

    UIntN level;
    EfiStatus status = MmuWalkLevel(PageDir, LastPhys, Virtual, shift, &level);

    if (EFI_ERROR(status) || EFI_ERROR((status = MmuWalkLevel((UInt64*)level, LastPhys, Virtual, shift - 9,
                                                              &level)))) {
        return status;
    } else if (!MmuSupports57) {
        /* On 48-bits, that's it, we can already call our internal Map2MB function (we already have the PD address). */

        MmuMap2MB((UInt64*)level, Virtual, Physical, NoExec);

        return EFI_SUCCESS;
    }

    /* On 57-bits we still have more one level (the PDP) to walk through. */

    if (EFI_ERROR((status = MmuWalkLevel((UInt64*)level, LastPhys, Virtual, 30, &level)))) {
        return status;
    }

    MmuMap2MB((UInt64*)level, Virtual, Physical, NoExec);

    return EFI_SUCCESS;
}

EfiStatus ArchInitCHicagoMmu(UInt16 Features, UIntN Start, UIntN MaxPhys, UIntN *LastVirt, UIntN *LastPhys,
                             UIntN *FrameBuffer, UIntN *RegionsVirt, UIntN *RegionsPhys, Void **Out) {
    if (LastVirt == Null || LastPhys == Null || FrameBuffer == Null || RegionsVirt == Null || RegionsPhys == Null ||
        Out == Null) {
        return EFI_INVALID_PARAMETER;
    }

    /* First, save if we support/should use PML5/57-bits virtual addressing. */

    if (Features & SIA_AMD64_57_BITS) {
        MmuHigherHalfStart = 0xFF00000000000000;
        MmuHigherHalfEnd = 0xFFFE000000000000;
        MmuSupports57 = True;
    }

    UInt64 rsize = (MaxPhys >> 22) / 0x90;

    /* Convert the address into a pointer (for the map function) and map the kernel + framebuffer (remembering to do
     * the cleanup if something fails, like arm64, and unlike x86). */

    *Out = (UInt64*)*LastPhys;
    *FrameBuffer = (MmuHigherHalfEnd - EfiGop->Mode->FrameBufferSize) & ~0x1FFFFF;

    /* Align UP the size of the RegionsStart... Region... */

    if (rsize & 0x1FFFFF) {
        rsize = (rsize + 0x200000) & ~0x1FFFFF;
    }

    *RegionsVirt = (*FrameBuffer - rsize) & ~0x1FFFFF;
    *RegionsPhys = *LastPhys;
    *LastPhys += rsize;

    /* We also need to map our jump function, as it will run (for a few instructions) with the new page directory. */

    EfiStatus status = MmuMap(*Out, LastPhys, (UIntN)ArchJumpIntoCHicago, (UIntN)ArchJumpIntoCHicago, False);

    if (EFI_ERROR(status)) {
        return status;
    }

    for (UIntN i = 0; i < EfiGop->Mode->FrameBufferSize; i += 0x200000) {
        if (EFI_ERROR((status = MmuMap(*Out, LastPhys, *FrameBuffer + i, EfiGop->Mode->FrameBufferBase + i, True)))) {
            return status;
        }
    }

    for (UIntN i = 0; i < rsize; i += 0x200000) {
        if (EFI_ERROR((status = MmuMap(*Out, LastPhys, *RegionsVirt + i, *RegionsPhys + i, True)))) {
            return status;
        }
    }

    for (UIntN i = Start; i < *LastPhys; i += 0x200000) {
        if (EFI_ERROR((status = MmuMap(*Out, LastPhys, MmuHigherHalfStart + i, i, False)))) {
            return status;
        }
    }

    ((UInt64*)*Out)[511] = ((UIntN)*Out & ~0xFFF) | MMU_PRESENT;

    return EFI_SUCCESS;
}
