/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(uint nbytes)
{
    register memblk *prev, *curr, *leftover;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (uint)roundmb(nbytes);
    struct memhead *head = (memhead *)proctab[currpid].heaptop;

    /* TODO:
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with remainder splitting)
     *      - if there is no suitable block, call user_incheap
     *        with the request to add more pages to our process heap
     *      - return memory address if successful
     */
    curr = pgfreelist->next;
    prev = pgfreelist;

    while(curr != NULL){
	uint currSize = *(uint*)((char*)curr-sizeof(uint));
	//exact fit
	if(currSize == nbytes){
		prev->next = curr->next;
		*(uint*)((char*)curr - sizeof(uint)) = nbytes;
		return (void*)curr;
	}
            // if matches the size requirements
        if(currSize > nbytes + sizeof(uint) + sizeof(struct pgmemblk)){
                leftover = (struct pgmemblk *)((char*) curr + sizeof(uint) + nbytes);
		uint leftoverSize = currSize - nbytes - sizeof(uint);

		*(uint*)((char*)leftover - sizeof(uint)) = leftoverSize;
		leftover->next = curr->next;

		prev->next = leftover;

		*(uint*)((char*)curr - sizeof(uint)) = nbytes;
		return (void*)curr;
        }

        // traversing list
        prev = curr;
        curr = curr->next;
    }

    // if no block, extend heap
    if (user_incheap(nbytes) == OK){
        return getmem(nbytes);
    }

    return (void *)SYSERR;
}
