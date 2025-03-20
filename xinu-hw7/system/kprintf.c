/**
 * COSC 3250 - Project 3
 * Creating getc, ungetc, and putc to give printing abilities to the OS
 * @author Paige Harrill and Kayla Imanzi
 * Instructor Dr. Brylow
 * TA-BOT:MAILTO @paige.harrill@marquette.edu @kayla.imanzi@marquette.edu
 */

#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX];
unsigned int  bufp;

syscall kgetc()
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;
    
    if(bufp!=0){ // checks if there is something in the ungetArray buffer
	--bufp; // if there is, decrease index
	return ungetArray[bufp];  //return the latest char from buffer
    }
    else{
    	// wait until the data ready signal is high
    	while((regptr->lsr & UART_LSR_DR)==0){}
    	return regptr->buffer;
    }
    return SYSERR;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    // if nothing in unget buffer, check UART buffer
    if(bufp==0){
    	if(regptr->lsr & UART_LSR_DR){return 1;}
	else {return 0;}
    }
    else{return 1;}
    return SYSERR; 
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
	// throw an error if buffer is full
    if(bufp >= UNGETMAX){
    	kprintf("ungetch: too many characters\n");
	return SYSERR;
    }
    else{
	   ungetArray[bufp] = c;  //put c into that buffer index
	   bufp++;  // increase bufp for next time
	   return c;  
    }
}

syscall kputc(unsigned char c)
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    // wait for the TX holding register to be empty to accept new data
    while (!(regptr->lsr & UART_LSR_THRE)){}
    // once that is true, put c into the buffer
    (regptr->buffer) = c;
    return c;
}

syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(long, long))kputc, 0);
    va_end(ap);
    return retval;
}
