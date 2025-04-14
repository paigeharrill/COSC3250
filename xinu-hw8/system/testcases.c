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

void print_freelist(void){
	struct pgmemblk *curr = pgfreelist->next;
	kprintf("==== Free List ====\n");
	while(curr != NULL){
		uint size = *(uint *)((char *)curr - sizeof(uint));
		kprintf("Blocked at %p, size: %u bytes\n", curr, size);
		curr = curr->next;
	}
	kprintf("=================\n");
}
/*testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;
	kprintf("0) Test malloc() and free()\r\n");
    	kprintf("1) Test getmem() and freemem()\r\n");
    	kprintf("2) Stress getmem/freemem and check freelist compaction\r\n");
	
	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':
			kprintf("Testing malloc() and free()");
			void *ptr1 = malloc(64);

			if(ptr1 != NULL){
				kprintf("Allocated 64 bytes at %p\n", ptr1);
				*((char *)ptr1) = 'X';
				kprintf("Vaue written to ptr1: %c\n", *((char *)ptr1));
				free(ptr1);
				kprintf("freed ptr1\n");
			}
			else{
				kprintf("malloc failed for 64 bytes\n");
			}

			void *ptr2 = malloc(128);
			void *ptr3 = malloc(256);
			kprintf("Allocated ptr2: %p\n", ptr2);
			kprintf("Allocated ptr3: %p\n", ptr3);
			free(ptr2);
			free(ptr3);
			kprintf("Freed ptr2 and ptr3\n");
			break;
		case '1':
			kprintf("Testing getmem() and freemem()\n");

			void *raw = getmem(100);
			if (raw != (void*)SYSERR){
				kprintf("getmem(100) returned %p\n", raw);
				freemem(raw, 100);
				kprintf("freemem successful\n");
			}
			else{
				kprintf("getmem failed\n");
			}
			break;
		case '2':
			kprintf("\nRunning memory allocator stress test...\n");

			print_freelist();

			void *a = getmem(64);
			void *b = getmem(128);
			void *c = getmem(256);
			kprintf("Allocated blocks:\na: %p\nb: %p\nc: %p\n", a, b, c);

			print_freelist();

			kprintf("Freeing b (middle block)\n");
			freemem(b, 128);
			print_freelist();

			kprintf("Freeing a (adjacent to b)\n");
			freemem(a, 64);
			print_freelist();

			kprintf("Freeing c\n");
			freemem(c, 256);
			print_freelist();

			kprintf("Allocating blocks until heap is full (watch for leaks)\n");
			void *blocks[100];
			int count = 0;

			while(1){
				void *block = getmem(512);
				if ((int)block == (int)SYSERR);{
					break;
				}
				blocks[count++] = block;
			}

			kprintf("Allocated %d blocks of 512 bytes\n", count);
			print_freelist();

			for(int i = 0; i<count; i++){
				freemem(blocks[i], 512);
			}
			kprintf("Freed all blocks.\n");
			print_freelist();
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
