/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 27 of 2021, at 22:53 BRT
 * Last edited on July 10 of 2021 at 17:43 BRT */

#include <arch.h>
#include <arch/mmu.h>
#include <efi/lib.h>

#define MMU_ENTRY_MASK(Level) 0xFFF
#define MMU_IS_HUGE(Entry) (Entry & MMU_HUGE)
#define MMU_IS_PRESENT(Entry) (Entry & MMU_PRESENT)
#define MMU_RECURSIVE_FLAGS (MMU_PRESENT | MMU_WRITE)
#define MMU_MAKE_TABLE(Physical, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | MMU_WRITE)

#ifdef _WIN64
#define MMU_TYPE UInt64

#define MMU_ENTRY_SIZE(Level) (Level == 2 ? 0x200000 : 0x1000)
#define MMU_NEED_TO_MOVE ((level == 2 && size < 0x200000) || i >= 512)
#define MMU_INDEX(Virtual, Level) (((Virtual) >> (!Level ? 39 : (Level == 1 ? 30 : (Level == 2 ? 21 : 12)))) & 0x1FF)
#define MMU_DEST_LEVEL(Virtual, Physical, Size) (!((Virtual) & 0x1FFFFF) && !((Physical) & 0x1FFFFF) && \
                                                 Size > 0x200000 ? 2 : 3)
#define MMU_MAKE_ENTRY(Physical, Flags, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | (Level == 2 ? MMU_HUGE : 0) | \
                                                ((Flags & MAP_EXEC) ? 0 : MMU_NO_EXEC | \
                                                ((Flags & MAP_WRITE) ? MMU_WRITE : 0)))
#else
#define MMU_TYPE UInt32

#define MMU_ENTRY_SIZE(Level) (!Level ? 0x400000 : 0x1000)
#define MMU_NEED_TO_MOVE ((!level && size < 0x400000) || i >= 1024)
#define MMU_INDEX(Virtual, Level) (((Virtual) >> (!Level ? 22 : 12)) & 0x3FF)
#define MMU_DEST_LEVEL(Virtual, Physical, Size) (!((Virtual) & 0x3FFFFF) && !((Physical) & 0x3FFFFF) && \
                                                 Size >= 0x400000 ? 0 : 1)
#define MMU_MAKE_ENTRY(Physical, Flags, Level) (((Physical) & ~0xFFF) | MMU_PRESENT | (!Level ? MMU_HUGE : 0) | \
                                                ((Flags & MAP_WRITE) ? MMU_WRITE : 0))
#endif

#include "../mmu.c"
