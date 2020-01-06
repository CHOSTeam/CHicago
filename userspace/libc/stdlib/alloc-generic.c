// File author is Ítalo Lima Marconato Matias
//
// Created on January 05 of 2020, at 22:16 BRT
// Last edited on January 05 of 2020, at 22:49 BRT

#define __ALLOC_GENERIC__
#include <chlibc/platform.h>

#include <stdlib.h>
#include <string.h>

static alloc_block_t *__alloc_base = NULL;
static __platform_lock_t __alloc_lock = -1;

static int __init_lock(Void) {
	return (__alloc_lock = __create_lock()) != -1;
}

static Void __split_block(alloc_block_t *block, size_t size) {
	if ((block == NULL) || (block->size <= (size + sizeof(alloc_block_t)))) {
		return;
	}
	
	alloc_block_t *new = (alloc_block_t*)(block->start + size);
	
	new->magic = ALLOC_BLOCK_MAGIC;
	new->size = block->size - (size + sizeof(alloc_block_t));
	new->start = block->start + size + sizeof(alloc_block_t);
	new->free = True;
	new->next = block->next;
	new->prev = block;
	
	block->size = size;
	block->next = new;
	
	if (new->next != NULL) {
		new->next->prev = new;
	}
}

static alloc_block_t *__fuse_block(alloc_block_t *block) {
	if ((block->next != NULL) && (((UINTPTR)block->next) == (block->start + block->size)) && (block->next->free)) {
		block->size += sizeof(alloc_block_t) + block->next->size;
		block->next = block->next->next;
		
		if (block->next != NULL) {
			block->next->prev = block;
		}
	}
	
	return block;
}

static alloc_block_t *__find_block(alloc_block_t **last, size_t size) {
	alloc_block_t *block = __alloc_base;
	
	while ((block != NULL) && (!((block->free) && (block->size >= size)))) {													// Check if the block is free and if it's equal or greater than specified size
		*last = block;																											// YES!
		block = block->next;
	}
	
	return block;
}

static alloc_block_t *__create_block(alloc_block_t *last, size_t size) {
	if (!size) {																												// We need a size...
		return NULL;
	}
	
	size_t tsize = size + sizeof(alloc_block_t);																				// Let's try to alloc!
	
	if ((tsize % ALLOC_PAGE_SIZE) != 0) {
		tsize += ALLOC_PAGE_SIZE - (size % ALLOC_PAGE_SIZE);
	}
	
	alloc_block_t *block = __alloc_virt(tsize);
	
	if (block == NULL) {
		return NULL;																											// Failed...
	}
	
	block->magic = ALLOC_BLOCK_MAGIC;
	block->size = tsize - sizeof(alloc_block_t);
	block->start = ((UINTPTR)block) + sizeof(alloc_block_t);
	block->free = False;
	block->next = NULL;
	block->prev = last;
	
	if (block->size > (size + sizeof(alloc_block_t))) {																			// Split this block if we can
		__split_block(block, size);
	}
	
	if (last != NULL) {
		last->next = block;
	}
	
	return block;
}

static int __check_block(UINTPTR block) {
	if (block == 0 || (block - sizeof(alloc_block_t) > block)) {																// First, null pointer check
		return False;
	}
	
	alloc_block_t *blk = (alloc_block_t*)(block - sizeof(alloc_block_t));														// Get the block struct
	
	return blk->magic != ALLOC_BLOCK_MAGIC;																						// Check the magic value is right
}

void *__alloc(size_t size) {
	if (__alloc_lock == -1 && !__init_lock()) {																					// Try to init the lock and do some checks...
		return 0;
	} else if (size == 0) {
		return 0;
	}
	
	__lock(__alloc_lock);																										// Lock
	
	alloc_block_t *block = __alloc_base;
	
	if ((size % 16) != 0) {																										// Align size to 16-bytes
		size += 16 - (size % 16);
	}
	
	if (__alloc_base != NULL) {
		alloc_block_t *last = __alloc_base;
		
		block = __find_block(&last, size);
		
		if (block != NULL) {																									// Found?
			if (block->size > (size + sizeof(alloc_block_t))) {																	// Yes! Let's try to split it (if we can)
				__split_block(block, size);
			}
			
			block->free = 0;																									// And... NOW THIS BLOCK BELONG TO US
		} else {
			block = __create_block(last, size);																					// No, so let's (try to) create a new block
			
			if (block == NULL) {
				__unlock(__alloc_lock);																							// Failed...
				return 0;
			}
		}
	} else {
		block = __alloc_base = __create_block(NULL, size);																		// Yes, so let's (try to) init
		
		if (block == NULL) {
			__unlock(__alloc_lock);																								// Failed...
			return 0;
		}
	}
	
	__unlock(__alloc_lock);																										// Unlock
	
	return (void*)block->start;
}

void *__aalloc(size_t align, size_t size) {
	if (__alloc_lock == -1 && !__init_lock()) {																					// Try to init the lock and do some checks...
		return 0;
	} else if (size == 0 || align == 0) {
		return 0;
	} else if ((align & (align - 1)) != 0) {
		return 0;
	}
	
	UINTPTR p0 = (UINTPTR)malloc(size + align + sizeof(void*) - 1);																// Alloc enough memory for all the trickery that we're going to do...
	
	if (p0 == 0) {
		return 0;
	}
	
	UINTPTR *p1 = (UINTPTR*)(p0 + (align - (p0 % align)));																		// Get the pointer to the start of the aligned area
	p1[-1] = p0;																												// Save the original pointer and return!
	
	return p1;
}

void __free(void *block) {
	if (__alloc_lock == -1 && !__init_lock()) {																					// Try to init the lock
		return;
	}
	
	__lock(__alloc_lock);																										// Lock
	
	if (!__check_block((UINTPTR)block)) {																						// The block is valid and need to be freed?
		if (__check_block(((UINTPTR*)block)[-1])) {																				// Let's check if this wasn't allocated by the aligned alloc function
			block = (void*)((UINTPTR*)block)[-1];																				// Yes, it is!
		} else {
			__unlock(__alloc_lock);
			return;
		}
	}
	
	alloc_block_t *blk = (alloc_block_t*)((UINTPTR)block - sizeof(alloc_block_t));												// Get the block struct
	
	if (blk->free) {
		__unlock(__alloc_lock);																									// We don't need to do anything...
		return;
	}
	
	blk->free = 1;
	
	while ((blk->prev != NULL) && (blk->prev->free)) {																			// Fuse with the prev
		blk = __fuse_block(blk->prev);
	}
	
	while ((blk->next != NULL) && (blk->next->free)) {																			// Fuse with the next
		__fuse_block(blk);
	}
	
	if (blk->next == NULL && ((UIntPtr)blk % ALLOC_PAGE_SIZE) == 0) {															// Free the virtual address?
		if (blk->prev != NULL) {																								// Yes! So let's free the end of the heap
			blk->prev->next = NULL;
		} else {
			__alloc_base = NULL;																								// No more blocks!
		}
		
		__free_virt(blk, blk->size + sizeof(alloc_block_t));																	// Return the memory to the system!
	}
	
	__unlock(__alloc_lock);																										// Unlock
}

void *__realloc(void *block, size_t size) {
	if (__alloc_lock == -1 && !__init_lock()) {																					// Try to init the lock
		return NULL;
	} else if (size == 0) {																										// Check if the size isn't zero
		return NULL;
	} else if (block == 0) {																									// NULL pointer block?
		return __alloc(size);																									// Yes, just redirect to the alloc func
	} else if (!__check_block((UINTPTR)block)) {																				// The block is valid and need to be freed?
		if (__check_block(((UINTPTR*)block)[-1])) {																				// Let's check if this wasn't allocated by the aligned alloc function
			block = (void*)((UINTPTR*)block)[-1];																				// Yes, it is!
		} else {
			return NULL;
		}
	}
	
	alloc_block_t *blk = (alloc_block_t*)((UINTPTR)block - sizeof(alloc_block_t));												// Get the block struct
	size_t sz = (blk->size > size) ? size : blk->size;																			// Save some extra info
	void *new = __alloc(size);																									// Alloc the new space
	
	if (new == NULL) {
		return NULL;
	}
	
	memcpy((void*)(((alloc_block_t*)(new - sizeof(alloc_block_t)))->start), (void*)blk->start, sz);								// Copy the old data
	__free(block);																												// Free the old address
	
	return new;
}
