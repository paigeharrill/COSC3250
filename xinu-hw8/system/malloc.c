/**
 * @file malloc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup libxc
 *
 * Request heap storage, record accounting information, returning pointer
 * to assigned memory region.
 *
 * @param size
 *      number of bytes requested
 *
 * @return
 *      pointer to region on success, or @c NULL on failure
 */
void *malloc(ulong size)
{
    struct memblock *pmem;

    /* we don't allocate 0 bytes. */
    if (0 == size)
    {
        return NULL;
    }

      /** TODO:
      *      1) Make room for accounting info
      *      2) Acquire memory with getmem
      *         - handle possible error (SYSERR) from getmem...
      *      3) Set accounting info in pmem
      */
    // making room for accounting info
    ulong total;

    total = sizeof(struct memblock) + size;
    pmem = getmem(total);
    if((ulong)pmem == SYSERR)
    { return NULL;}

    pmem->length = total;
    pmem->next = NULL;

    return (void *)((char *)pmem + sizeof(struct memblock));
}
