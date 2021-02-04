/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 22:53 BRT
 * Last edited on January 31 of 2021 at 21:20 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

static Void MmuMap(UInt32 *PageDir, UIntN Virtual, UIntN Physical) {
    /* To calculate the index into the page directory, we have to take out the first 22 bits (divide by 4MB) and
     * truncate the result to a value between 0-1023. */

    PageDir[(Virtual >> 22) & 0x3FF] = (Physical & ~0x3FFFFF) | MMU_PRESENT | MMU_HUGE;
}

EfiStatus ArchInitCHicagoMmu(UInt16, UIntN Start, UIntN MaxPhys, UIntN *LastVirt, UIntN *LastPhys, UIntN *FrameBuffer,
                             UIntN *RegionsVirt, UIntN *RegionsPhys, Void **Out) {
    if (LastVirt == Null || LastPhys == Null || FrameBuffer == Null || RegionsVirt == Null || RegionsPhys == Null ||
        Out == Null) {
        return EFI_INVALID_PARAMETER;
    }

    /* Convert the address into a pointer (for the map function) and map the kernel + framebuffer. */

    UIntN rsize = (MaxPhys >> 22) / 0x88;

    *Out = (Void*)*LastPhys;
    *LastPhys += 0x1000;
    *LastVirt += 0x1000;
    *FrameBuffer = (0xFF800000 - EfiGop->Mode->FrameBufferSize) & ~0x3FFFFFFF;

    if (rsize & 0x3FFFFFFF) {
        rsize = (rsize + 0x400000) & ~0x3FFFFFFF;
    }

    *RegionsVirt = (*FrameBuffer - rsize) & ~0x3FFFFFFF;
    *RegionsPhys = *LastPhys;
    *LastPhys += rsize;

    /* Here the mapping of the jump function will not fail lol (as we don't alloc anything). */

    MmuMap(*Out, (UIntN)ArchJumpIntoCHicago, (UIntN)ArchJumpIntoCHicago);

    for (UIntN i = 0; i < EfiGop->Mode->FrameBufferSize; i += 0x400000) {
        MmuMap(*Out, *FrameBuffer + i, EfiGop->Mode->FrameBufferBase + i);
    }

    for (UIntN i = 0; i < rsize; i += 0x400000) {
        MmuMap(*Out, *RegionsVirt + i, *RegionsPhys + i);
    }

    for (UIntN i = Start; i < *LastPhys; i += 0x400000) {
        MmuMap(*Out, 0xC0000000 + i - Start, i);
    }

    return EFI_SUCCESS;
}
