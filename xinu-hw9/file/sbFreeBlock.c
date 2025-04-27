/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <xinu.h>

devcall swizzle(int diskfd, struct fbcnode *fbc){
	struct fbcnode *fbc2 = fbc->fbc_next;

	if(fbc2 = NULL){
		fbc->fbc_next = 0;
	}else{
		fbc->fbc_next = fbc->fbc_next->fbc_blocknum;
	}

	seek(diskfd, fbc->fbc_blocknum);
	if(SYSERR == write(diskfd, fbc, sizeof(struct fbcnode))){
		return SYSERR;
	}

	// reset freeblocks fbc_next
	fbc->fbc_next = fbc2;

	return OK;
	// if collector node == NUL
	// 	collector node fbc_next = 0
	// else
	// 	collector node's next = next's block num
	// seek
	// write
}

devcall swizzleSuperBlock(int diskfd, struct superblock *psuper){
	struct fbcnode *swizzle = psuper->sb_freelst;
	struct dirblock *swizzle2 = psuper->sb_dirlst;

	// overwrite sb_freelist to be freelist's blocknum
	// overwrite sb_dirlist to be sb_dirlist's blocknum
	
	psuper->sb_freelst = psuper->sb_freelst->fbc_blocknum;
	psuper->sb_dirlst = psuper->sb_dirlst->db_blocknum;

	seek(diskfd, psuper->sb_blocknum);
	if(SYSERR == write(diskfd, psuper, sizeof(struct superblock))){
		return SYSERR;
	}

	// revert copies
	psuper->sb_freelst = swizzle;
	psuper->sb_dirlst = swizzle2;

	return OK;
}
/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block)
{
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.

    int diskfd;
    struct dentry *phw;

    //errorcheck if superblock is null

    if(psuper == NULL){
    	return SYSERR;
    }

    phw = psuper->sb_disk;
    //errorcheck != null

    if(phw == NULL){
    	return SYSERR;
    }
    //error check block we got as a param (! <= 0 && !> total blocks on disk)

    if(block <= 0 || block > psuper->sb_blocktotal){
    	return SYSERR;
    }

    diskfd = phw-devtab;
    //wait

    wait(psuper->sb_freelock);

    //first collector node
    struct fbcnode *fbc = psuper->sb_freelst;

    // case: if the disk is completely full
    	// free up 1 block, make it a collector node
	
    if(fbc == NULL){
    	struct fbcnode *newBlock = malloc(sizeof(struct fbcnode));
	newBlock->fbc_count = 0;
	newBlock->fbc_blocknum = block;

	// add newBlock to psuper
	psuper->sb_freelst = newBlock;

	// write info to disk
	if(SYSERR == swizzleSuperBlock(diskfd, psuper)){
		signal(psuper->sb_freelock);
		return SYSERR;
	}

	if(SYSERR == swizzle(diskfd, psuper->sb_freelst)){
		signal(psuper->sb_freelock);
		return SYSERR;
	}

	signal(psuper->sb_freelock);
	return OK;
    }
	// while loop to get the last collector node
        //      fcb->fcb_next

    while(fbc->fbc_next != NULL){
    	fbc = fbc->fbc_next;
    }

    // case: the free block collector node is completely full or completely empty
    	// create new free block collector node, add to the first spot in it

    if(fbc->fbc_count >= FREEBLOCKMAX || fbc->fbc_count == 0 && psuper->sb_freelst == fbc){
    	// make new collector block node
	struct fbcnode *newCollector;
	newCollector = (struct fbcnode *)malloc(sizeof(struct fbcnode));
	if(newCollector == NULL){
		signal(psuper->sb_freelock);
		return SYSERR;
	}

	newCollector->fbc_count = 0;
	newCollector->fbc_blocknum = block;
	newCollector->fbc_next = NULL;

	// add to first spot
	fbc->fbc_next = newCollector;

	// write to disk
	if(SYSERR == swizzle(diskfd, fbc)){
		signal(psuper->sb_freelock);
		return SYSERR;
	}
	if(SYSERR == swizzle(diskfd, newCollector)){
		signal(psuper->sb_freelock);
		return SYSERR;
	}

	signal(psuper->sb_freelock);
	return OK;
    }
    // case: add to the next available index in collector node
    	// put our block into the next spot in the array
	// write this information to the disk
    if(fbc->fbc_count < FREEBLOCKMAX){
    	// add to next available index in collector node
	// put block into next spot in array
    	fbc->fbc_free[fbc->fbc_count] = block;
	fbc->fbc_count++;

	// write to disk
	if(SYSERR == swizzle(diskfd, fbc)){
		signal(psuper->sb_freelock);
		return SYSERR;
	}

	signal(psuper->sb_freelock);
	return OK;
    }
    return SYSERR;
}
