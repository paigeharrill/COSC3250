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
    
    block->length = nbytes;
    block->next = NULL;

    //traverse free list to find point of insertion
    prev = NULL;
    next = head->head;

    // traverse to find correct position in address of free list
    while(next != NULL && (ulong)next < (ulong)block){
        prev = next;
        next = next->next;
    }

    //overlap checking, check with previous block
    if (prev != NULL)
    {
	    top = (ulong)prev + prev->length;
	    if (top > (ulong)block)
	    { return SYSERR;}
    }

    //Check next block
    top = (ulong)block + nbytes;
    if (next != NULL)
    {
	    if (top > (ulong) next)
	    { return SYSERR;}
    }

    //merge with previous if adjacent
    if (prev != NULL && ((ulong)prev + prev->length == (ulong)block))
    {
	    prev->length += nbytes;
	    block = prev;
    }
    else
    {
	    //insert block into free list
	    block->next = next;
	    if (prev == NULL){
		    head->head = block;
	    }
	    else{
		    prev->next = block;
	    }
    }

    //merge with next block if adjacent
    if (next != NULL && ((ulong)block + block->length == (ulong)next))
    {
	    block->length += next->length;
	    block->next = next->next;
    }
    //update free list total mem
    head->length += nbytes;

    return OK;
}
