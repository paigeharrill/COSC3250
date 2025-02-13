/**
 * @file testcases.c
 * @provides testcases
 *
 * Modified by: @paige.harrill@marquette.edu
 *
 * and @kayla.imanzi@marquette.edu
 *
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;
    int result;

    kprintf("===TEST BEGIN===\r\n");

   /* kprintf("Testing kputc and kgetc...\r\n");
    c = kputc('A');
   
    
    if (c == 'A'){
    	kprintf("\nPASS: kput/kgetc working. \r\n");
    }
    else{
    	kprintf("FAIL: kput/kgetc returned unexpected character: %c\r\n", c);
    }

    kprintf("Testing kungetc...\r\n");
    result = kungetc('B');
    if(result == 'B'){
    	c = kgetc();
	if (c == 'B'){
		kprintf("PASS: kungetc and kgetc working properly.\r\n");
	}
	else {
		kprintf("Fail: kgetc did not retreive expected character.\r\n");
	}
    }
    
    else{
    	kprintf("FAIL: kungetc failed to store character.\r\n");
    }

    kprintf("Testing kungetc buffer limit...\r\n");
    for (int i = 0; i < 10; i++)
    {
        kungetc('X'); // fill the buffer
    }
    result = kungetc('Y'); // this should fail
    if (result == SYSERR)
    {
        kprintf("PASS: kungetc correctly detected buffer overflow.\r\n");
    }
    else
    {
        kprintf("FAIL: kungetc allowed too many characters.\r\n");
    }

    // test kcheckc: ensure it detects available characters
    kprintf("Testing kcheckc...\r\n");
    kungetc('C');  // put a character in the buffer
    if (kcheckc())
    {
        kprintf("PASS: kcheckc correctly detected available character.\r\n");
    }
    else
    {
        kprintf("FAIL: kcheckc did not detect available character.\r\n");
    }
    
    //default:
    //    kprintf("Hello Xinu World!\r\n");
    //}*/
    
    // this is a simple kcheckc test, prints infinitely and turns from 0 -> 1 when it detects ur input
    //while(1){
    //	    kprintf("%d\n\r", kcheckc());
   // }

    // lastly, testing to see that when we enter a character it is received
    c = kgetc();
    kprintf("You entered %c",c);
    kprintf("\r\n===TEST END===\r\n");
    
    return;
}
