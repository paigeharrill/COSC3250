/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

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
    //printf("Beginning file delete FD = %d\n", fd); 
    // error check file number (fd) and superblock
    if(fd < 0 || fd > FILEMAX){
    	return SYSERR;
    }

    // given
    struct dentry *phw = supertab->sb_disk;
    int diskfd = phw - devtab;

    //printf("Wating on sb_dirblock\n");
    // waiting on sb_dirblock
    wait(supertab->sb_dirlock);

    //printf("Waited on block\n");
    // error check sb_dirlist
    if((NULL == supertab) || (NULL == filetab) || (NULL == supertab->sb_dirlst)){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }

    //printf("Past sb_dirlst\n");
    // error check file state
    if(isbadfd(fd)){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }

    //printf("Resetting file values\n");
    // reset values of file
    filetab[fd].fn_state = FILE_FREE;
    filetab[fd].fn_cursor = 0;
    filetab[fd].fn_length = 0;
    filetab[fd].fn_name[0] = '\0';
    free(filetab[fd].fn_data);

    //printf("Removing data from hard drive\n");
    // remove data from hard drive with sbFreeBlock()
    if(SYSERR == sbFreeBlock(supertab, filetab[fd].fn_blocknum)){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }

    //update hard drive info with seek and write
    seek(diskfd, supertab->sb_dirlst->db_blocknum);
    if(SYSERR == write(diskfd, supertab->sb_dirlst, sizeof(struct dirblock))){
    	signal(supertab->sb_dirlock);
	return SYSERR;
    }

    // signal semaphore
    signal(supertab->sb_dirlock);
    return OK;
}
