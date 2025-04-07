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

    curr = freelist.next;
    prev = (memblk *)&freelist;

    while(curr != NULL){
	    // if matches the size requirements
    	if(curr->length >= nbytes){
		prev->next = curr.next;
		freelist.length -= nbytes;

		curr->next = (memblk*)&curr->next; // mark allocated
		return (void*)(curr + 1);	// return pointer
	} // split the block if there will be leftover
       	else if(curr->length > nbytes){
		leftover = (memblk*)((ulong)curr + nybtes);
		leftover->length = curr->length - nbytes;
		leftover->next = curr->next;

		prev->next = leftover;
		freelist.length -= nbytes;

		curr->length = nbytes;
		curr->next = (memblk *)&curr->next;	// mark allocated
		return (void*)(curr + 1);	// return pointer
	}

	// traversing list
	prev = curr;
	curr = curr->next
    }

    // if no block, extend heap
    if (user_incheap(nbytes) == OK){
    	return getmem(nbytes);
    }


    return (void *)SYSERR;
}
