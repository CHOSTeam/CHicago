// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2018, at 20:53 BRT
// Last edited on January 05 of 2020, at 22:33 BRT

#define __ALLOC_INT__
#include <chicago/alloc.h>
#include <chicago/process.h>
#include <chicago/string.h>
#include <chicago/virt.h>

static PAllocBlock MmAllocBase = Null;
static IntPtr MmAllocLock = -1;

static Boolean MmInitLock(Void) {
	return (MmAllocLock = PsCreateLock()) != -1;
}

static Void MmSplitBlock(PAllocBlock block, UIntPtr size) {
	if ((block == Null) || (block->size <= (size + sizeof(AllocBlock)))) {
		return;
	}
	
	PAllocBlock new = (PAllocBlock)(block->start + size);
	
	new->magic = ALLOC_BLOCK_MAGIC;
	new->size = block->size - (size + sizeof(AllocBlock));
	new->start = block->start + size + sizeof(AllocBlock);
	new->free = True;
	new->next = block->next;
	new->prev = block;
	
	block->size = size;
	block->next = new;
	
	if (new->next != Null) {
		new->next->prev = new;
	}
}

static PAllocBlock MmFuseBlock(PAllocBlock block) {
	if ((block->next != Null) && (((UIntPtr)block->next) == (block->start + block->size)) && (block->next->free)) {
		block->size += sizeof(AllocBlock) + block->next->size;
		block->next = block->next->next;
		
		if (block->next != Null) {
			block->next->prev = block;
		}
	}
	
	return block;
}

static PAllocBlock MmFindBlock(PAllocBlock *last, UIntPtr size) {
	PAllocBlock block = MmAllocBase;
	
	while ((block != Null) && (!((block->free) && (block->size >= size)))) {													// Check if the block is free and if it's equal or greater than specified size
		*last = block;																											// YES!
		block = block->next;
	}
	
	return block;
}

static PAllocBlock MmCreateBlock(PAllocBlock last, UIntPtr size) {
	if (!size) {																												// We need a size...
		return Null;
	}
	
	UIntPtr tsize = size + sizeof(AllocBlock);																					// Let's alloc aligned to the page size
	
	if ((tsize % MM_PAGE_SIZE) != 0) {
		tsize += MM_PAGE_SIZE - (size % MM_PAGE_SIZE);
	}
	
	PAllocBlock block = (PAllocBlock)VirtAllocAddress(0, tsize, VIRT_PROT_READ | VIRT_PROT_WRITE);								// Let's try to alloc some space!
	
	if (block == Null) {
		return Null;																											// Failed...
	}
	
	block->magic = ALLOC_BLOCK_MAGIC;
	block->size = tsize - sizeof(AllocBlock);
	block->start = ((UIntPtr)block) + sizeof(AllocBlock);
	block->free = False;
	block->next = Null;
	block->prev = last;
	
	if (block->size > (size + sizeof(AllocBlock))) {																			// Split this block if we can
		MmSplitBlock(block, size);
	}
	
	if (last != Null) {
		last->next = block;
	}
	
	return block;
}

static Boolean MmIsValidBlock(UIntPtr block) {
	if (block == 0 || (block - sizeof(AllocBlock) > block)) {																	// First, null pointer check
		return False;
	}
	
	PAllocBlock blk = (PAllocBlock)(block - sizeof(AllocBlock));																// Get the block struct
	
	return blk->magic != ALLOC_BLOCK_MAGIC;																						// Check the magic value is right
}

UIntPtr MmAllocMemory(UIntPtr size) {
	if (MmAllocLock == -1 && !MmInitLock()) {																					// Try to init the lock and do some checks...
		return 0;
	} else if (size == 0) {
		return 0;
	}
	
	PsLock(MmAllocLock);																										// Lock
	
	PAllocBlock block = MmAllocBase;
	
	if ((size % 16) != 0) {																										// Align size to 16-bytes
		size += 16 - (size % 16);
	}
	
	if (MmAllocBase != Null) {
		PAllocBlock last = MmAllocBase;
		
		block = MmFindBlock(&last, size);
		
		if (block != Null) {																									// Found?
			if (block->size > (size + sizeof(AllocBlock))) {																	// Yes! Let's try to split it (if we can)
				MmSplitBlock(block, size);
			}
			
			block->free = False;																								// And... NOW THIS BLOCK BELONG TO US
		} else {
			block = MmCreateBlock(last, size);																					// No, so let's (try to) create a new block
			
			if (block == Null) {
				PsUnlock(MmAllocLock);																							// Failed...
				return 0;
			}
		}
	} else {
		block = MmAllocBase = MmCreateBlock(Null, size);																		// Yes, so let's (try to) init
		
		if (block == Null) {
			PsUnlock(MmAllocLock);																								// Failed...
			return 0;
		}
	}
	
	PsUnlock(MmAllocLock);																										// Unlock
	
	return block->start;
}

UIntPtr MmAllocAlignedMemory(UIntPtr size, UIntPtr align) {
	if (MmAllocLock == -1 && !MmInitLock()) {																					// Try to init the lock and do some checks...
		return 0;
	} else if (size == 0 || align == 0) {
		return 0;
	} else if ((align & (align - 1)) != 0) {
		return 0;
	}
	
	UIntPtr p0 = MmAllocMemory(size + align + sizeof(PVoid) - 1);																// Alloc enough memory for all the trickery that we're going to do...
	
	if (p0 == 0) {
		return 0;
	}
	
	PUIntPtr p1 = (PUIntPtr)(p0 + (align - (p0 % align)));																		// Get the pointer to the start of the aligned area
	p1[-1] = p0;																												// Save the original pointer and return!
	
	return (UIntPtr)p1;
}

Void MmFreeMemory(UIntPtr block) {
	if (MmAllocLock == -1 && !MmInitLock()) {																					// Try to init the lock
		return;
	}
	
	PsLock(MmAllocLock);																										// Lock
	
	if (!MmIsValidBlock(block)) {																								// The block is valid and need to be freed?
		if (MmIsValidBlock(((PUIntPtr)block)[-1])) {																			// Let's check if this wasn't allocated by MmAllocAlignedMemory
			block = ((PUIntPtr)block)[-1];																						// Yes, it is!
		} else {
			PsUnlock(MmAllocLock);
			return;
		}
	}
	
	PAllocBlock blk = (PAllocBlock)(block - sizeof(AllocBlock));																// Get the block struct
	
	if (blk->free) {
		PsUnlock(MmAllocLock);																									// We don't need to do anything...
		return;
	}
	
	blk->free = True;
	
	while ((blk->prev != Null) && (blk->prev->free)) {																			// Fuse with the prev
		blk = MmFuseBlock(blk->prev);
	}
	
	while ((blk->next != Null) && (blk->next->free)) {																			// Fuse with the next
		MmFuseBlock(blk);
	}
	
	if (blk->next == Null && ((UIntPtr)blk % MM_PAGE_SIZE) == 0) {																// Free the virtual address?
		if (blk->prev != Null) {																								// Yes! So let's free the end of the heap
			blk->prev->next = Null;
		} else {
			MmAllocBase = Null;																									// No more blocks!
		}
		
		VirtFreeAddress((UIntPtr)blk, blk->size + sizeof(AllocBlock));															// Return the memory to the system!
	}
	
	PsUnlock(MmAllocLock);																										// Unlock
}

UIntPtr MmReallocMemory(UIntPtr block, UIntPtr size) {
	if (MmAllocLock == -1 && !MmInitLock()) {																					// Try to init the lock
		return 0;
	} else if (size == 0) {																										// Check if the size isn't zero
		return 0;
	} else if (block == 0) {																									// Null pointer block?
		return MmAllocMemory(size);																								// Yes, just redirect to MmAllocMemory
	} else if (!MmIsValidBlock(block)) {																						// The block is valid and need to be freed?
		if (MmIsValidBlock(((PUIntPtr)block)[-1])) {																			// Let's check if this wasn't allocated by MmAllocAlignedMemory
			block = ((PUIntPtr)block)[-1];																						// Yes, it is!
		} else {
			return 0;
		}
	}
	
	PAllocBlock blk = (PAllocBlock)(block - sizeof(AllocBlock));																// Get the block struct
	UIntPtr sz = (blk->size > size) ? size : blk->size;																			// Save some extra info
	UIntPtr new = MmAllocMemory(size);																							// Alloc the new space
	
	if (new == 0) {
		return 0;
	}
	
	StrCopyMemory((PUInt8)(((PAllocBlock)(new - sizeof(AllocBlock)))->start), (PUInt8)blk->start, sz);							// Copy the old data
	MmFreeMemory(block);																										// Free the old address
	
	return new;
}

UIntPtr MmGetAllocSize(UIntPtr block) {
	if (MmAllocLock == -1 && !MmInitLock()) {																					// Try to init the lock
		return 0;
	} else if (!MmIsValidBlock(block)) {																						// The block is valid and need to be freed?
		if (MmIsValidBlock(((PUIntPtr)block)[-1])) {																			// Let's check if this wasn't allocated by MmAllocAlignedMemory
			block = ((PUIntPtr)block)[-1];																						// Yes, it is!
		} else {
			return 0;
		}
	}
	
	return ((PAllocBlock)(block - sizeof(AllocBlock)))->size;																	// Return the size
}
