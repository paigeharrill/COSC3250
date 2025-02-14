/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>
#include <proc.h>

static pid_typ newpid(void);
void userret(void);
void *getstk(ulong);

#define ARG_REG_MAX 8

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, char *name, ulong nargs, ...)
{
    ulong *saddr;               /* stack address                */
    ulong pid;                  /* stores new process id        */
    pcb *ppcb;                  /* pointer to proc control blk  */
    ulong i;
    va_list ap;                 /* points to list of var args   */
    ulong pads = 0;             /* padding entries in record.   */

    if (ssize < MINSTK)
        ssize = MINSTK;

    ssize = (ulong)((((ulong)(ssize + 3)) >> 2) << 2);
    /* round up to even boundary    */
    saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
    pid = newpid();
    /* a little error checking      */
    if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
    {
        return SYSERR;
    }

    numproc++;
    ppcb = &proctab[pid];
	
    // TODO: Setup PCB entry for new process.
	// feeling good on this one
    ppcb->state = PRSUSP;		// defined in header file for suspended state
    ppcb->stkbase = (ulong *)(saddr - ssize); 	// base of stack (bottom) = stack address - size of stack
    ppcb->stklen = ssize;		// stack size is ssize
    strncpy(ppcb->name, name, PNMLEN); 	// strncpy(pointer to string (pcbr name), parameter (create() arg for name), length)
    ppcb->stkptr = NULL;
    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = pid;
    *--saddr = ppcb->stklen;
    *--saddr = (ulong)ppcb->stkbase;

    /* Handle variable number of arguments passed to starting function   */
    if (nargs)
    {
        pads = ((nargs - 1) / ARG_REG_MAX) * ARG_REG_MAX;
    }
    /* If more than 8 args, pad record size to multiple of native memory */
    /*  transfer size.  Reserve space for extra args                     */
    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }
    // TODO: Initialize process context.
    
    //setting each register to 0 so that its memory is allocated and we know whats in it
    for(int i = 0; i < 32; i++){
    	*--saddr = 0;
    }

    ppcb->stkptr = saddr;  // assign stack pointer with stack address


    // TODO:  Place arguments into context and/or activation record.
    //        See K&R 7.3 for example using va_start, va_arg and
    //        va_end macros for variable argument functions.
    
    va_start(ap, nargs);

    //assigning the args to registers
    for(int i =0; i<nargs; i++){
    	// for 8 args, A0-A7, assign as arg
	if(i<8){
		saddr[CTX_A0+i] = va_arg(ap, ulong);
	}
	else{ //padding
		saddr[CTX_PC + (i-7)] = va_arg(ap, ulong);
	}
    }
    va_end(ap);

    //assign program counter, register addr, and stack pointer
    saddr[CTX_SP] = &saddr[CTX_PC];
    saddr[CTX_PC] = funcaddr;
    saddr[CTX_RA] = &userret;

    return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
    pid_typ pid;                /* process id to return     */
    static pid_typ nextpid = 0;

    for (pid = 0; pid < NPROC; pid++)
    {                           /* check all NPROC slots    */
        nextpid = (nextpid + 1) % NPROC;
        if (PRFREE == proctab[nextpid].state)
        {
            return nextpid;
        }
    }
    return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
    // ASSIGNMENT 5 TODO: Replace the call to kill(); with user_kill();
    // when you believe your trap handler is working in Assignment 5
    // user_kill();
    kill(currpid); 
}
