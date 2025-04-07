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
void *getmem(ulong nbytes)
{
    register memblk *prev, *curr, *leftover;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);
    struct memhead *head = (memhead *)PROCHEAPADDR;

    /* TODO:
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with remainder splitting)
     *      - if there is no suitable block, call user_incheap
     *        with the request to add more pages to our process heap
     *      - return memory address if successful
     */

    curr = *head;
    prev = NULL;
    memhead *freehead = &freelist;

    while(curr != NULL){
	    // if matches the size requirements
    	if(curr->length >= nbytes){
		// create leftover block of size whatever we don't need
		leftover = (struct memblock *)(nbytes + ((ulong)curr));
		leftover->length = (curr->length) - nbytes;

		// length of the free list
		freehead->length = freehead->length - nbytes;

		// set next of leftover to next from current and previous to the leftover
		leftover->next = curr->next;
		prev->next = leftover;

		// set length of current to nbytes (which was requested)
		curr->length = nbytes;
		
		// return the current which is now the right size
		return (curr);
	}
	// traversing list
	prev = curr;
	curr = curr->next
    }


    return (void *)SYSERR;
}
