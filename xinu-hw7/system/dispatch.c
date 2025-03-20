/**
 * @file dispatch.c
 * @provides dispatch
 *
 * COSC 3250 Assignment 5
 * @author Paige Harrill & Kayla Imanzi
 * Instructor: Dr. Brylow
 * TA-BOT:MAILTO paige.harrill@marquette.edu kayla.imanzi@marquette.edu
 **/

/* Embedded XINU, Copyright (C) 2008, 2023.  All rights reserved. */


#include <xinu.h>

/**
 * @ingroup process
 * Dispatch the trap or exception handler, called via interrupt.S
 * @param cause  The value of the scause register 
 * @param stval  The value of the stval register  
 * @param frame  The stack pointer of the process that caused the interupt 
 * @param program_counter  The value of the sepc register 
 */

void dispatch(ulong cause, ulong val, ulong *frame, ulong *program_counter) {
    ulong swi_opcode;
    
    if((long)cause > 0) {
        cause = cause << 1;
        cause = cause >> 1;

	// if call is from user mode
       	if(cause == E_ENVCALL_FROM_UMODE){
		swi_opcode = frame[CTX_A7]; //grab this because syscall file
	 	frame[CTX_A0] = syscall_dispatch(swi_opcode, (ulong*)&frame[CTX_A0]); //args start at a0, put back in	
		set_sepc(program_counter+4);	// increment by a word
	}
	else if((cause==E_ENVCALL_FROM_SMODE) ||(cause==E_ENVCALL_FROM_MMODE)){
		xtrap(frame, cause, val, program_counter); // if from smode or mmode, go to trap
	} 
	/**
	* TODO:
	* Check to ensure the trap is an environment call from U-Mode
	* Find the system call number that's triggered
	* Pass the system call number and any arguments into syscall_dispatch.
	* Make sure to set the return value in the appropriate spot.
	* Update the program counter appropriately with set_sepc
	*
	* If the trap is not an environment call from U-Mode call xtrap
	*/
    }
    else{
    	cause = cause << 1;
	cause = cause >> 1;
	uint irq_num;

	volatile uint *int_sclaim = (volatile uint *)(PLIC_BASE + 0x201004);
    	irq_num = *int_sclaim;

    	if(cause == I_SUPERVISOR_EXTERNAL) {
        	interrupt_handler_t handler = interruptVector[irq_num];

        	*int_sclaim = irq_num;
        	if (handler)
        	{
            		(*handler) ();
        	} else {
            		kprintf("ERROR: No handler registered for interrupt %u\r\n",
                    	irq_num);

            		while (1);
        	}
    	}
    }
}

