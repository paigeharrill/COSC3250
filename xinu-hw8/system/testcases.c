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
void setup_heap(void){
	proctab[0].heaptop = pgalloc();
	memhead *mh = (memhead *)proctab[0].heaptop;
	mh->head = (mh+1);
	mh->length = PAGE_SIZE - sizeof(struct memhead);
	mh->base = mh->head;
	mh->bound = ((ulong)mh) + PAGE_SIZE;
	mh->head->next = NULL;
	mh->head->length = mh->length;
}


void print_freelist(void){
	struct memhead *head = (memhead *)proctab[currpid].heaptop;
	if (head == NULL)
	{
		kprintf("Error: heap header is null\n\r");
		return;
	}
	kprintf("\n==== Free List ====\n\r");
	struct memblock *curr = head->head;
	uint size;
	//kprintf("after curr->head\n\r");
	if (curr == NULL)
	{
		kprintf("Free List is empty. \n\r");
	}

	while(curr != NULL){
		size = curr->length;
		kprintf("Block at %x, size: %u bytes\n\r", curr, size);
		curr = curr->next;
	}
	kprintf("=================\n\r");
}


//Helper function to count number of free blocks

int count_free_blocks(void)
{
	int count = 0;
	struct memhead *head = (struct memhead *)proctab[currpid].heaptop;
	struct memblock *curr = head->head;
	
	while (curr != NULL)
	{
		count ++;
		curr = curr->next;
	}
	return count;
}

/*testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;
	kprintf("0) Test malloc() and free()\r\n");
    	kprintf("1) Test getmem() and freemem()\r\n");
    	kprintf("2) Stress getmem/freemem\r\n");
	kprintf("3) Test freelist compaction\r\n");
	kprintf("4) Print freelist\r\n");
	kprintf("5) Extra testing for malloc()\r\n");
	
	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':{
			kprintf("Testing malloc() and free()\n\r");
			setup_heap();
			kprintf("Initial free list: \n\r");
			print_freelist();

			void *ptr = malloc(128);
			if (ptr == NULL){
				kprintf("malloc() failed");
				return;
			}
			else 
			{
				kprintf("malloc allocated 128 bytes at %x\n\r", (ulong)ptr);
			}
			print_freelist();
			kprintf("Test free() after using malloc()\n\r");
			//call free
			free(ptr);
			kprintf("Free() was called.\n\r");
			kprintf("Free list after free: \n\r");
			print_freelist();
			break;}

		case '1':{
			kprintf("Testing getmem() and freemem()\n\r");
			setup_heap();
			kprintf("\nInitial free list: \n\r");
			print_freelist();

			void *raw = getmem(100);
			if (raw != (void*)SYSERR){
				kprintf("\ngetmem(100) returned %x\n\r", (ulong)raw);
				print_freelist();
				if(freemem(raw, 100) ==SYSERR){
					kprintf("\nfreemem() failed to free block at %x\n\r", (ulong)raw);
					return;
				}
				else{
					kprintf("\nfreemem successful. Freed 100 bytes from %x\n\r", (ulong)raw);
					print_freelist();
				}
			}
			else{
				kprintf("getmem failed\n");
			}
			break; }


		case '2':{
			kprintf("\ntest various allocations and frees...\n");

			setup_heap();
			kprintf("Initial free list: \r\n");
			print_freelist();

			//Array of different sizes to request
			uint sizes[] = {50, 100, 250, 500, 1024};
			int num_sizes = sizeof(sizes)/ sizeof(uint);
			void *allocated_blocks[num_sizes];

			//Allocate memory blocks for each size
			for (int i = 0; i < num_sizes; i++) {
				allocated_blocks[i] = getmem(sizes[i]);
				if (allocated_blocks[i] == (void *)SYSERR) {
					kprintf("Error: getmem() failed to allocate %u bytes.\n", sizes[i]);
				} else {
					kprintf("Allocated %u bytes (rounded to %u) at 0x%08lx.\n\r", sizes[i], (uint)roundmb(sizes[i]), (ulong)allocated_blocks[i]);
				}
				print_freelist();
			}

			//Free each allocated block
			for (int i = 0; i < num_sizes; i++) {
				if (allocated_blocks[i] != (void *)SYSERR) {
					if (freemem(allocated_blocks[i], (uint)roundmb(sizes[i])) == SYSERR) {
						kprintf("Error: freemem() failed to free block allocated for %u bytes.\n", sizes[i]);
					} else {
						kprintf("Freed block allocated for %u bytes (rounded to %u bytes).\n\r", sizes[i], (uint)roundmb(sizes[i]));
					}
					print_freelist();
				}
			}
			break;}


		
		case '3':{
			kprintf("\n--Test freemem() compaction\n\r");
			setup_heap();
			struct memhead *head = (struct memhead *)proctab[currpid].heaptop;
			ulong initial_free = head->length;
			kprintf("Initial free memory: %lu bytes\n", initial_free);

			//Allocate 2 blocks
			void *block1 = getmem(200);
			void *block2 = getmem(300);
			if (block1 == (void *)SYSERR || block2 == (void *)SYSERR)
			{
				kprintf("Allocation failed \n\r");
				return;
			}
			kprintf("After allocations:\n\r");
			print_freelist();
			int free_blocks_after_alloc = count_free_blocks();
			kprintf("Number of free blocks after allocation: %d\n\r", free_blocks_after_alloc);
			//free first scond block, then first block. round block sizes
			if (freemem(block2, (uint)roundmb(300)) == SYSERR)
			{
				kprintf("Error freeing block 2,\n\r");
			}
			if (freemem(block1, (uint) roundmb(200)) == SYSERR)
			{
				kprintf("Error freeing block1.\n");
			}
			else
			{
				kprintf("Freed block1 (200 bytes).\n");
			}

			print_freelist();
			int free_blocks_after_free = count_free_blocks();
			kprintf("Number of free blocks after freeing: %d\n\r", free_blocks_after_free);

			//check of compaction occurred
			//expected: one free block, total memory  = initial memory
			if (free_blocks_after_free == 1 && head->length == initial_free)
			{
				kprintf("Compaction succeeded: free list merged into one block.\n");
			}
			else
			{
				kprintf("Compaction failed or did not merge as expected.\n");
			}
			break;}
					
		case '4':{
			setup_heap();
			print_freelist();
			break;}

		case '5':{
			 kprintf("\n Maalloc test case\n\r");

			 //set up heap
			 setup_heap();
			 print_freelist();
			 
			 //Test that malloc returns null
			 void *p0 = malloc(0);
			 if(p0 == NULL) {
				 kprintf("malloc(0) correctly returned NULL.\n\r");
			 } else {
				 kprintf("Error: malloc(0) did not return NULL; returned %x\n\r", (ulong)p0);
			 }

			 //define sizes to allocate
			 uint sizes[] = {1, 50, 100, 200, 1024};
			 int numSizes = sizeof(sizes) / sizeof(uint);
			 void *allocated[numSizes];

			 //Allocate memory blocks for each size
			 for (int i = 0; i < numSizes; i++) {
				 allocated[i] = malloc(sizes[i]);
				 if (allocated[i] == NULL) {
					 kprintf("Error: malloc(%u) returned NULL.\n\r", sizes[i]);
				 } else {
					 kprintf("malloc(%u) returned : %x\n\r", sizes[i], (ulong)allocated[i]);
				 }
			 }

			 //print free list after malloc
			 kprintf("Free lost after malloc allocations: \n\r");
			 print_freelist();

			 //free each block
			 for (int i = 0; i < numSizes; i++) {
				 if (allocated[i] != NULL) {
					 free(allocated[i]);
					 kprintf("Freed block allocated for %u bytes (pointer: %x).\n\r", sizes[i], (ulong)allocated[i]);
				 }
			 }

			 //print free list after freeing all blocks
			 kprintf("Final free list after freeing all malloc allocations:\n\r");
			 print_freelist();
			 break;}


		default:
			break;
	}
	while(numproc>1){
		resched();
	}	
	kprintf("\r\n===TEST END===\r\n");
	return;
}
