/**
 * @file dispatch.c
 * @provides dispatch
 *
 */
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

       	if(cause == E_ENVCALL_FROM_UMODE){
		swi_opcode = frame[CTX_A7];
	 	frame[CTX_A0] = syscall_dispatch(swi_opcode, (ulong*)frame[CTX_A0]);	
		set_sepc(program_counter+4);
	}
	else if((cause==E_ENVCALL_FROM_SMODE) ||(cause==E_ENVCALL_FROM_MMODE)){
		xtrap(frame, cause, val, program_counter);
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
}

