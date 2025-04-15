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
*
*
*/
	
    //traverse free list using first-fit strategy
    prev = NULL;
    curr = head->head;

    while (curr != NULL){
	    if (curr->length >= nbytes)
	    {
		    //if current block is larger, split into allocate and leftover block
		    if((curr->length - nbytes) >= sizeof(memblk))
		    {
			    //create new free block from remainder
			    //new block staring nbytes after start of curr
			    leftover = (memblk *)((char *)curr + nbytes);
			    leftover->length = curr->length - nbytes;
			    leftover->next = curr->next;

			    //replace curr in free list with leftover
			    if (prev == NULL){
				    head->head = leftover;
			    }
			    else
			    {
				    prev->next = leftover;
			    }

			    //update free list's total length
			    head->length -= nbytes;
			    curr->length = nbytes;
		    }
		    else
		    {		
			    //if block is just big enough or leaves too small fragment, remove enitre block
			    if (prev == NULL){
				    head->head = curr-> next;
			    }
			    else{
				    prev->next = curr->next;
			    }
			    head->length -= curr->length;
		    }
		    return (void *) curr;
	    }
		    prev = curr;
		    curr = curr->next;
    }

    //no suitable block found in free list: extend heap
    {

    	    //compute size to extend heap by
	    uint extend_size = roundpage(nbytes);
	    void *newchunk = user_incheap(nbytes);

	    if ((ulong)newchunk == SYSERR || newchunk == NULL)
	    { return (void *)SYSERR;}

	    //create new memblk in the newly allocated heap space
	   memblk *newblk = (memblk *)newchunk;
	   newblk->length = extend_size;
	   newblk->next = head->head;

	   //add new block to free list
	   head->head = newblk;
	   head->length += newblk->length;

	   //recursively try getmem()
	   return getmem(nbytes);
    }
}
