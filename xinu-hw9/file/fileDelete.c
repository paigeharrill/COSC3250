/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/* Paige Harrill                                                                */
/* and                                                             */
/* Kayla Imanzi                                                                */
/* TA-BOT:MAILTO paige.harrill@marquette.edu kayla.imanzi@marquette.edu                                                                */

#include <xinu.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

	// Error check file number (fd) and superblock

    if(fd == NULL){
    	return SYSERR;
    }

    // given
    struct dentry *phw = supertab->sb_disk; // found in file.h
    int diskfd = phw-devtab;

    //wait on our sb_dirblock
    wait(supertab->sb_dirlock);

    //error check sb_dirlist
    if((NULL==supertab) || (NULL == filetab) || (NULL == supertab->sb_dirlst)){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }
    //error check file state
    if(isbadfd(fd)){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }
    //reset values of file (very similar to file create)

    filetab[fd].fn_state = FILE_FREE;
    filetab[fd].fn_cursor = 0;
    filetab[fd].fn_length = 0;
    filetab[fd].fn_name[0] = '\0';
    free(filetab[fd].fn_data);

    //remove data from hard drive with sbFreeBlock()
    if(SYSERR == sbFreeBlock(supertab, filetab[fd].fn_blocknum)){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }

    //error check
    
    //update hard drive info with seek and write
    	// seek (where we are trying to write to, index of how far we are writing to)
	// write (where we are trying to write to, what we are trying to write, size of what we are trying to write)

    seek(diskfd, supertab->sb_dirlst->db_blocknum);
    if(SYSERR == write(diskfd, supertab->sb_dirlst, sizeof(struct dirblock))){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }
    // signal semaphore
    signal(supertab->sb_dirlock);
    return OK;
}
