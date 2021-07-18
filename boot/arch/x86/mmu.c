/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 22:53 BRT
 * Last edited on July 18 of 2021 at 15:59 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

#define MMU_TYPE UInt64
#define MMU_ENTRY_MASK(Level) 0xFFF
#define MMU_IS_HUGE(Entry) (Entry & MMU_HUGE)
#define MMU_IS_PRESENT(Entry) (Entry & MMU_PRESENT)
#define MMU_RECURSIVE_FLAGS (MMU_PRESENT | MMU_WRITE)

#ifdef _WIN64
#define MMU_ENTRY_SIZE(Level) (Level == 2 ? 0x200000 : 0x1000)
#define MMU_NEED_TO_MOVE ((level == 2 && size < 0x200000) || i >= 512)
#define MMU_MAKE_TABLE(Physical, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | MMU_WRITE)
#define MMU_DEST_LEVEL(Virtual, Physical, Size) (!((Virtual) & 0x1FFFFF) && !((Physical) & 0x1FFFFF) && \
                                                 Size >= 0x200000 ? 3 : 3)
#define MMU_INDEX(Virtual, Level) (((Virtual) >> (!Level ? 39 : (Level == 1 ? 30 : (Level == 2 ? 21 : 12)))) & 0x1FF)
#define MMU_MAKE_ENTRY(Physical, Flags, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | (Level == 2 ? MMU_HUGE : 0) | \
                                                ((Flags & MAP_EXEC) ? 0 : MMU_NO_EXEC | \
                                                ((Flags & MAP_WRITE) ? MMU_WRITE : 0)))
#else
#define MMU_ENTRY_SIZE(Level) (Level == 1 ? 0x200000 : 0x1000)
#define MMU_NEED_TO_MOVE ((level == 1 && size < 0x200000) || i >= 512)
#define MMU_MAKE_TABLE(Physical, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | (Level ? MMU_WRITE : 0))
#define MMU_DEST_LEVEL(Virtual, Physical, Size) (!((Virtual) & 0x1FFFFF) && !((Physical) & 0x1FFFFF) && \
                                                 Size >= 0x200000 ? 1 : 2)
#define MMU_INDEX(Virtual, Level) (!Level ? (((Virtual) >> 30) & 0x03) : \
                                            (((Virtual) >> (Level == 1 ? 21 : 12)) & 0x1FF))
#define MMU_MAKE_ENTRY(Physical, Flags, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | (Level == 1 ? MMU_HUGE : 0) | \
                                                ((Flags & MAP_EXEC) ? 0 : MMU_NO_EXEC | \
                                                ((Flags & MAP_WRITE) ? MMU_WRITE : 0)))

#define MMU_SETUP_RECURSIVE() \
    for (UIntN i = 0; i < 4; i++) { \
        if (pd[i] & MMU_PRESENT) continue; \
        if ((list = AddMapping(*List, UINTN_MAX, &addr, 0, 0x1000, 0)) == Null || !addr) return EFI_OUT_OF_RESOURCES; \
        EfiZeroMemory((Void*)addr, 0x1000); \
        pd[i] = MMU_MAKE_TABLE(addr, 0); \
        *List = list; \
    } \
    \
    MMU_TYPE *rpd = (MMU_TYPE*)(pd[3] & ~0xFFF); \
    \
    rpd[508] = pd[0] | MMU_WRITE; \
    rpd[509] = pd[1] | MMU_WRITE; \
    rpd[510] = pd[2] | MMU_WRITE; \
    rpd[511] = pd[3] | MMU_WRITE; \
    \
    return EFI_SUCCESS;

#endif

#include "../mmu.c"
