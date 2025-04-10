/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by: Paige Harrill & Kayla Imanzi	
c *
 * TA-BOT:MAILTO paige.harrill@marquette.edu kayla.imanzi@marquette.edu
 *
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

/* This fake page table will allow you to test your printPageTable function
 * without having paging completely working.
 */
void test_useraccess(void){
	kprintf("Attempting to read a kernel variable\n\r");
	extern uint bufp;
        kprintf("Bufp: %u\n\r", bufp);
        kprintf("Attempting to modify variable\n\r");
        bufp = 1234; // should fail
        kprintf("Updated: %u\n\r", bufp);
}

int test_usernone(void) {
        kprintf("This is a test of ...");
        user_none();
        kprintf("user_none() syscall\r\n");

        return 0;
}

pgtbl createFakeTable(void){
	pgtbl root = pgalloc();
	pgtbl lvl1 = pgalloc();
	pgtbl lvl0 = pgalloc();

	volatile ulong *pte = &(root[5]);
	*pte = PA2PTE(lvl1) | PTE_V;

	ulong *lvl1pte = &(lvl1[145]);
	*lvl1pte = PA2PTE(lvl0) | PTE_V;

	ulong *lvl0pte = &(lvl0[343]);
	*lvl0pte = PA2PTE(0x1000) | PTE_W | PTE_R | PTE_V;

	ulong *lvl0pte1 = &(lvl0[120]);
	*lvl0pte1 = PA2PTE(0x4000) | PTE_X | PTE_R | PTE_V;

	ulong *lvl0pte2 = &(lvl0[45]);
	*lvl0pte2 = PA2PTE(0x8000) | PTE_X | PTE_R | PTE_V;

	return root;
}

void printPageTable(pgtbl pagetable)
{
	/*
	* TODO: Write a function that prints out the page table.
	* Your function should print out all *valid* page table entries in
	* the page table.  If any of the entries are a link (if the
	* Read/Write/Execute bits aren't set), recursively print that page
	* table.  If it is a leaf, print the page table entry and the
	* physical address is maps to. 
	*/
	static int level = 0;
	//kprintf("in print function\n\r");
	for (int i = 0; i < 512; i++){
		char R = '-', W = '-', X = '-';
		if(pagetable[i] & PTE_V){
			ulong entry = pagetable[i];
			ulong phys_addr = PTE2PA(entry);
			if(entry & PTE_R){
				R = 'R';
			}
			if(entry & PTE_W){
				W = 'W';
			}
			if(entry & PTE_X){
				X = 'X';
			}
			if(entry & (PTE_R | PTE_W | PTE_X)){
				kprintf("%*sEntry %d -> PA: 0x%lx \t%c %c %c\n\r", level*4, "", i, phys_addr, R, W, X);
			}
			else{
				kprintf("%*sEntry %d -> Next level table at 0x%lx\n\r", level*4, "",  i, phys_addr);
				level++;
				printPageTable((pgtbl)phys_addr);
				level--;
			}
		}
	}
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;
	kprintf("0) Test print page table\r\n");
    	kprintf("1) Test user process restricted memory access\r\n");
    	kprintf("2) Test kernel permissions\r\n");
	kprintf("3) Test null pointer exception\r\n");
	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':
			//pgtbl samplePage = createFakeTable();
			//printPageTable(samplePage);
			pid_typ pid = create((void *)test_usernone, INITSTK, 1, "test_usernone", 0);
			ready(pid, RESCHED_NO);
			printPageTable(proctab[pid].pagetable);
			// TODO: Write a testcase that creates a user process
			// and prints out it's page table
			break;
		case '1':
			kprintf("\nTesting user process restricted memory access\n");
			void (*user_func)(void) = (void (*)(void))0xFFFFFFFF80000000;
			kprintf("Trying to execute user function at 0xFFFFFFFF80000000\n");
			user_func(); // should trigger exception
			kprintf("This shouldn't print");
			// TODO: Write a testcase that demonstrates a user
			// process cannot access certain areas of memory
			break;
		case '2':
			kprintf("\nTesting kernel permissions\n\r");
			pid_typ pid2 = create((void*)test_useraccess, INITSTK, 10, "test_useraccess", 0);
			ready(pid2, RESCHED_YES);
			// TODO: Write a testcase that demonstrates a user
			// process can read kernel variables but cannot write
			// to them
			break;
		case '3':
			kprintf("\nTesting Null Pointer Exception\n");
			int *null_ptr = NULL;
			kprintf("Accessing NULL pointer\n");
			int value = *null_ptr;
			kprintf("Value at NULL: %d\n", value); // should break;
			// TODO: Extra credit! Add handling in xtrap to detect
			// and print out a Null Pointer Exception.  Write a
			// testcase that demonstrates your OS can detect a
			// Null Pointer Exception.
			break;
		default:
			break;
	}
	while(numproc>1){
		resched();
	}	
	kprintf("\r\n===TEST END===\r\n");
	return;
}
