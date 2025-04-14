/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with getmem().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to getmem().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall freemem(void *memptr, uint nbytes)
{
    register struct memblock *block, *next, *prev;
    struct memhead *head = NULL;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes)
        || ((ulong)memptr < (ulong)proctab[currpid].heaptop))
    {
        return SYSERR;
    }

    head = (struct memhead *)proctab[currpid].heaptop;
    block = (struct memblock *)memptr;
    nbytes = (uint)roundmb(nbytes);

    /* TODO:
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     */
    next = (struct memblock *)pgfreelist->next;
    prev = (struct memblock *)&pgfreelist;

    // traverse to find correct position in address ordered free list
    while(next != NULL && (ulong)next < (ulong)block){
        prev = next;
        next = next->next;
    }

    // calculate top of the block to be freed
    top = (ulong)block+sizeof(uint)+nbytes;

    ulong prevTop = (ulong)prev+sizeof(uint)+*(uint*)((char*)prev);

    // overlap with previous block
    if (prevTop > (ulong)block){
        return SYSERR;
    }

    // overlap with next block
    if ((next != NULL) && (top > (ulong)next)){
        return SYSERR;
    }

    // coalesce with next block if adjacent
    if ((next != NULL) && (top == (ulong)next)){
        nbytes += sizeof(uint) + *(uint*)((char*)next);
	*(uint*)block = nbytes;
	block->next = next->next;
    } else{
        *(uint*)block = nbytes;
	block->next = next;
    }

    // coalesce with previous block if adjacent
    if (prevTop == (ulong)block) {
        *(uint *)((char *)prev) += sizeof(uint) + *(uint *)block;
	prev->next = block->next;
    } else {
        prev->next = block;
    }

    return OK;
}
