/**
 * @file testcases.c
 * @provides testcases
 *
 * TA-BOT:MAILTO
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

extern void main(int, char *);

int test_usernone(void) {
	kprintf("This is a test of ...");
	user_none();
	kprintf("user_none() syscall\r\n");

	return 0;
}

int test_userputc(void) {
	user_putc(0, 'H');
	user_putc(0, 'e');
	user_putc(0, 'l');
	user_putc(0, 'l');
	user_putc(0, 'o');
	user_putc(0, ' ');
	user_putc(0, 'W');
	user_putc(0, 'o');
	user_putc(0, 'r');
	user_putc(0, 'l');
	user_putc(0, 'd');
	user_putc(0, '!');
	user_putc(0, '\r');
	user_putc(0, '\n');

	return 0;
}

int test_usergetc(void) {
	char c;
	kprintf("Echo characters until 'X': ");
	while ((c = user_getc(0)) != 'X')
	{
		user_putc(0, c);
	}
	kprintf("\r\n");

	return 0;
}

int testmain(int argc, char **argv)
{
    int i = 0;
    kprintf("Hello XINU World!\r\n");

    for (i = 0; i < 10; i++)
    {
        kprintf("This is process %d\r\n", currpid);

        user_yield();
    }
    return 0;
}

void testbigargs(int a, int b, int c, int d, int e, int f, int g, int h)
{
    kprintf("Testing bigargs...\r\n");
    kprintf("a = 0x%08X\r\n", a);
    kprintf("b = 0x%08X\r\n", b);
    kprintf("c = 0x%08X\r\n", c);
    kprintf("d = 0x%08X\r\n", d);
    kprintf("e = 0x%08X\r\n", e);
    kprintf("f = 0x%08X\r\n", f);
    kprintf("g = 0x%08X\r\n", g);
    kprintf("h = 0x%08X\r\n", h);
}

void printpcb(int pid)
{
    pcb *ppcb = NULL;

    /* Using the process ID, access it in the PCB table. */
    ppcb = &proctab[pid];

    /* Printing PCB */
    kprintf("Process name		  : %s \r\n", ppcb->name);

    switch (ppcb->state)
    {
    case PRFREE:
        kprintf("State of the process	  : FREE \r\n");
        break;
    case PRCURR:
        kprintf("State of the process 	  : CURRENT \r\n");
        break;
    case PRSUSP:
        kprintf("State of the process	  : SUSPENDED \r\n");
        break;
    case PRREADY:
        kprintf("State of the process	  : READY \r\n");
        break;
    default:
        kprintf("ERROR: Process state not correctly set!\r\n");
        break;
    }

    /* Print PCB contents and registers */
    kprintf("Base of run time stack    : 0x%08X \r\n", ppcb->stkbase);
    kprintf("Stack length of process   : %8u \r\n", ppcb->stklen);
}

// first attempt at testing preemption in following 3 functions
void bigdog(void) {
	kprintf("Big dog running...\r\n");
	while(1)
		;
}

void littledog(void) {
	kprintf("Little dog runing...\r\n");
	while(1)
		;
}

void test_preemption(void) {
	int bigdog_pid, littledog_pid;

	kprintf("Starting pre-emption test...\r\n");

	//create bigdogprocess
	bigdog_pid = create((void *)bigdog, INITSTK, 1, "bigdog", 0);

	//create littledog process
	littledog_pid = create((void *)littledog, INITSTK, 1, "littledog", 0);

	//run bigdog first
	ready(bigdog_pid, RESCHED_YES);

	//run littledog after
	ready(littledog_pid, RESCHED_YES);

	//wait for processes to finish
	while (numproc > 1)
		resched();
	    
	kprintf("Pre-emption test complete.\r\n");
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;

    kprintf("0) Test user_none syscall\r\n");
    kprintf("1) Test user_getc syscall\r\n");
    kprintf("2) Test user_putc syscall\r\n");
    kprintf("3) Create three processes that test user_yield syscall\r\n");
    kprintf("P) Test preemption \r\n");


    kprintf("===TEST BEGIN===\r\n");

    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
    case '0':
        ready(create((void *)test_usernone, INITSTK, 1, "test_usernone", 0),
              RESCHED_YES);
		break;

    case '1':
        ready(create((void *)test_usergetc, INITSTK, 1, "test_usergetc", 0),
              RESCHED_YES);
        break;

    case '2':
        ready(create((void *)test_userputc, INITSTK, 1, "test_userputc", 0),
              RESCHED_YES);
		break;

    case '3':
        // Create three copies of a process, and let them play.
        ready(create((void *)testmain, INITSTK, 1, "MAIN1", 2, 0, NULL),
              RESCHED_NO);
        ready(create((void *)testmain, INITSTK, 1, "MAIN2", 2, 0, NULL),
              RESCHED_NO);
        ready(create((void *)testmain, INITSTK, 1, "MAIN3", 2, 0, NULL),
              RESCHED_YES);
        while (numproc > 1)
            resched();
        break;

    case 'P':
	// testing preemption and watching what happens
	ready(create((void *)testbigargs, INITSTK, 15, "BigArgs", 8,0x00000000, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 
			0x55555555, 0x66666666, 0x77777777, 0x88888888),RESCHED_NO);

	ready(create((void *)testmain, INITSTK, 5, "MAIN4", 2, 0, NULL),  RESCHED_NO);
	
	while (numproc>1)
		resched();
	
	break;


    default:
        break;
    }

    kprintf("\r\n===TEST END===\r\n");
    return;
}
