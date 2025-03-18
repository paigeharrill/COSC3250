/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 Assignment 6
 * Authors: Paige Harrill & Kayla Imanzi
 * TA-BOT:MAILTO paige.harrill@marquette.edu kayla.imanzi@marquette.edu
 */
/* Embedded XINU, Copyright (C) 2008,2024.  All rights reserved. */

#include <xinu.h>

ulong totalTickets(void);
extern void ctxsw(void *, void *);

// finds the total number of tickets in the currently ready processes
ulong totalTickets(void){
	ulong total = 0;
	int i = 0;

	// goes through total number of processes, if ready, gets num of tickets and adds to total
	for(i = 0; i<NPROC; i++){
		if((&proctab[i])->state == PRREADY){
			total += (&proctab[i])->tickets;		
		}
	}
	return total;
}

int pickWin(ulong total){
	ulong winner;
	int counter = 0;
	int i;

	winner = random(total);

	if(winner == 0){
		winner += 1;
	}

	for(i = 0; i < NPROC; i++){
		if((&proctab[i])->state == PRREADY){
			counter += (&proctab[i])->tickets;

			if(counter >= winner){
				return i;
			}
		}
	}
}
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */

    oldproc = &proctab[currpid];
    ulong total = 0;

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        enqueue(currpid, readylist);
    }

    /**
     * We recommend you use a helper function for the following:
     * TODO: Get the total number of tickets from all processes that are in
     * current and ready states.
     * Use random() function to pick a random ticket. 
     * Traverse through the process table to identify which proccess has the
     * random ticket value.  Remove process from queue.
     * Set currpid to the new process.
     */
    total = totalTickets();
    currpid = pickWin(total);
    remove(currpid);
    //currpid = dequeue(readylist);
    newproc = &proctab[currpid];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt = QUANTUM;
#endif

    ctxsw(&oldproc->ctx, &newproc->ctx);

    /* The OLD process returns here when resumed. */
    return OK;
}
