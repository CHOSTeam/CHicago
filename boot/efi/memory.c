/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 15 of 2021, at 20:09 BRT
 * Last edited on March 11 of 2021 at 17:43 BRT */

#include <efi/lib.h>

/* Generic SetMemory8/16/32/64 functions, we should later try optimizing this for each architecture (so that we use
 * SSE on x86/amd64, for example). */

#define EFI_SET_MEM(N) \
Void *EfiSetMemory##N(Void *Buffer, UIntN Size, UInt##N Data) {\
    UInt##N *buf = Buffer;\
\
    while (Size--) {\
        *buf++ = Data;\
    }\
\
    return Buffer;\
}

#define EFI_DUP_STR(N) \
Char##N *EfiDuplicateString##N(const Char##N *String) {\
    UIntN len = 0; \
    while (String[len++]) ; \
    return EfiAllocateCopyPool(String, (len + 1) * sizeof(Char##N)); \
}

#define EFI_COMP_STR(N) \
Boolean EfiCompareString##N(const Char##N *Left, const Char##N *Right) {\
    while (*Left && *Right) {\
        if (*Left++ != *Right++) {\
            return False;\
        }\
    }\
\
    return *Left == *Right;\
}

EFI_DUP_STR(8);
EFI_DUP_STR(16);

EFI_SET_MEM(8);
EFI_SET_MEM(16);
EFI_SET_MEM(32);
EFI_SET_MEM(64);

EFI_COMP_STR(8);
EFI_COMP_STR(16);

IntN EfiToLower(IntN Character) {
    return Character >= 'A' && Character <= 'Z' ? Character + 0x20 : Character;
}

Void *EfiZeroMemory(Void *Buffer, UIntN Size) {
    return EfiSetMemory8(Buffer, Size, 0);
}

Void *EfiCopyMemory(Void *Dest, const Void *Source, UIntN Size) {
    UInt8 *dst = Dest, *src = (UInt8*)Source;

    while (Size--) {
        *dst++ = *src++;
    }

    return Dest;
}

Boolean EfiCompareMemory(const Void *Buf1, const Void *Buf2, UIntN Size) {
    const UInt8 *buf1 = Buf1, *buf2 = Buf2;

    while (Size--) {
        if (*buf1++ != *buf2++) {
            return False;
        }
    }

    return True;
}

Void *EfiAllocatePool(UIntN Size) {
    /* The type for the loader allocations (both Pool and Pages) will always be LoaderData, and we don't need to return
     * the status code, we can just return the allocated region (or Null if it failed). */

    Void *ret;
    return EFI_ERROR(EfiBS->AllocatePool(EfiLoaderData, Size, (Void**)&ret)) ? Null : ret;
}

Void *EfiAllocateZeroPool(UIntN Size) {
    /* Just if it isn't obvious: ZeroPool allocates some memory and cleans it/set it to zero, CopyPool allocs and copy
     * the data from another allocation into the new one (the size of the new one needs to be equal or less than the
     * old one), and ReallocatePool allocates a new buffer, copy the data of the old one into the new one, and free the
     * old one. */

    Void *ret = EfiAllocatePool(Size);

    if (ret != Null) {
        EfiZeroMemory(ret, Size);
    }

    return ret;
}

Void *EfiAllocateCopyPool(const Void *Source, UIntN Size) {
    Void *ret = EfiAllocatePool(Size);

    if (ret != Null && Source != Null) {
        EfiCopyMemory(ret, Source, Size);
    }

    return ret;
}

Void *EfiReallocatePool(Void *Source, UIntN OldSize, UIntN NewSize) {
    Void *ret = EfiAllocateZeroPool(NewSize);

    if (ret != Null && Source != Null) {
        EfiCopyMemory(ret, Source, OldSize < NewSize ? OldSize : NewSize);
        EfiFreePool(Source);
    }

    return ret;
}

EfiPhysicalAddress EfiAllocatePages(UIntN Pages) {
    EfiPhysicalAddress ret;
    return EFI_ERROR(EfiBS->AllocatePages(EfiAllocateAnyPages, EfiLoaderData, Pages, &ret)) ? 0 : ret;
}

Void EfiFreePool(Void *Pool) {
    /* FreePool (and FreePages) DO return a status value, but we're going to ignore it here (for now). */

    if (Pool != Null) {
        EfiBS->FreePool(Pool);
    }
}

Void EfiFreePages(EfiPhysicalAddress Start, UIntN Pages) {
    EfiBS->FreePages(Start, Pages);
}

EfiMemoryDescriptor *EfiGetMemoryMap(UIntN *Count, UIntN *Key, UIntN *Size, UInt32 *Version) {
    /* This function is simillar to EfiGetFileSize, as in we need to call the same function twice (one for getting the
     * memory map size, and one for actually reading the memory map).  */

    if (Count == Null) {
        return Null;
    }

    UIntN size = 0;
    EfiMemoryDescriptor *map = Null;
    EfiStatus status = EfiBS->GetMemoryMap(&size, map, Key, Size, Version);


    /* Also, let's overallocate a bit, as allocating the mmap space itself will probably create some more entries. */

    size += *Size * 2;

    if (status != EFI_BUFFER_TOO_SMALL) {
        return Null;
    } else if ((map = EfiAllocatePool(size)) == Null) {
        return Null;
    } else if (EFI_ERROR((status = EfiBS->GetMemoryMap(&size, map, Key, Size, Version)))) {
        EfiFreePool(map);
        return Null;
    }

    *Count = size / *Size;

    return map;
}
