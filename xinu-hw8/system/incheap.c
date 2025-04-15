/**
 * @file incmem.c
 * @provides incmem
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

extern void *end;

static struct memhead *fetchmhead(pcb *ppcb)
{
    // Map the virtual address of the heap head to physical
    pgtbl ppt = (pgtbl) PTE2PA(ppcb->pagetable[PX(2, PROCHEAPVADDR)]);
    ppt = (pgtbl) PTE2PA(ppt[PX(1, PROCHEAPVADDR)]);
    return (struct memhead *)PTE2PA(ppt[PX(0, PROCHEAPVADDR)]);
}

/**
 * Increase the heap of a user process, returning bottom virtual address
 * of the new memory region.
 * @param size amount of memory to allocate, in bytes
 * @return bottom virtual address of the new memory region
 */
void *incheap(ulong size)
{
    pcb *ppcb;                  /* pointer to proc control blk  */
    ppcb = &proctab[currpid];
    ulong vtop = 0;
    ulong vaddr = 0;
    ulong vend = 0;
    page pg = NULL;

    // Round size to the next multiple of PAGE_SIZE
    size = roundpage(size);
    struct memhead *mh = fetchmhead(ppcb);
    vtop = vaddr = mh->bound;
    vend = vtop + size;

    // Need a stack check
    if (vend > (ulong)PROCSTACKVADDR)
    {
        return (void *)SYSERR;
    }

    // Loop over the entire range
    for (; vaddr < vend; vaddr += PAGE_SIZE)
    {
        if ((pg = pgalloc()) == (ulong *)SYSERR)
        {
            return ((void *)SYSERR);
        }

        if ((mapAddressRange
             (ppcb->pagetable, vaddr, (ulong)pg, PAGE_SIZE,
              PTE_R | PTE_W | PTE_U | PTE_A | PTE_D)) == SYSERR)
        {
            return ((void *)SYSERR);
        }
    }
    mh->bound = vend;
    return ((ulong *)vtop);
}
