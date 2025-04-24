/* sbOpen.c - sbOpen */
/* Copyright (C) 2007, Marquette University.  All rights reserved. */

#include <xinu.h>

/*------------------------------------------------------------------------
 * sbOpenDirectory - Read in a directory block for a filesystem.
 *------------------------------------------------------------------------
 */
local sbOpenDirectory(int diskfd, int firstblk)
{
    struct dirblock *dirblk = NULL;
    int i = 0;

    if (0 == firstblk)
    {
        return NULL;
    }

    // Reconstruct directory list in memory.
    dirblk = (struct dirblock *)malloc(sizeof(struct dirblock));
    if (NULL == dirblk)
    {
        return NULL;
    }
    seek(diskfd, firstblk);
    read(diskfd, dirblk, sizeof(struct dirblock));
    filetab = dirblk->db_fnodes;
    for (i = 0; i < DIRENTRIES; i++)
    {
        filetab[i].fn_state &= FILE_USED;
    }
    return (int)dirblk;
}

/*------------------------------------------------------------------------
 * sbOpenFreeList - Read in a filesystem free list.
 *------------------------------------------------------------------------
 */
local sbOpenFreeList(int diskfd, int firstblk)
{
    struct fbcnode *fbc = NULL, *fbc2 = NULL, *head = NULL;

    if (0 == firstblk)
    {
        return NULL;
    }

    // Reconstruct free list in memory.
    fbc = (struct fbcnode *)malloc(sizeof(struct fbcnode));
    if (NULL == fbc)
    {
        return NULL;
    }
    head = fbc2 = fbc;
    seek(diskfd, firstblk);
    read(diskfd, fbc, sizeof(struct fbcnode));
    while (NULL != fbc->fbc_next)
    {
        fbc2 = (struct fbcnode *)malloc(sizeof(struct fbcnode));
        if (NULL == fbc2)
        {
            return SYSERR;
        }
        seek(diskfd, (int)fbc->fbc_next);
        read(diskfd, fbc2, sizeof(struct fbcnode));
        fbc->fbc_next = fbc2;
        fbc = fbc2;
    }
    return (int)head;
}

/*------------------------------------------------------------------------
 * sbOpen - Given a disk device, open a superblock or initialize one.
 *------------------------------------------------------------------------
 */
devcall sbOpen(struct disk *pdisk)
{
    struct superblock sbdisk;
    struct superblock *psuper = NULL;
    int result;
    int diskfd;

    // The usual paranoid error checking.
    if (NULL == pdisk)
    {
        return SYSERR;
    }
    diskfd = pdisk->disk_pdev - devtab;

    psuper = supertab =
        (struct superblock *)malloc(sizeof(struct superblock));
    if (NULL == psuper)
    {
        return SYSERR;
    }

    result = seek(diskfd, SUPERBLOCKNUM);
    if (OK != result)
    {
        return SYSERR;
    }

    result = read(diskfd, &sbdisk, sizeof(struct superblock));
    if (result < 0)
    {
        return SYSERR;
    }

    // Set up some defaults for the superblock.
    psuper->sb_magic = DISKMAGIC;
    psuper->sb_disk = devtab + diskfd;
    psuper->sb_freelst = NULL;
    psuper->sb_freelock = semcreate(1);
    psuper->sb_dirlst = NULL;
    psuper->sb_dirlock = semcreate(1);

    // Check for a valid superblock.
    if (DISKMAGIC == sbdisk.sb_magic)
    {
        printf
            ("Filesystem superblock at %d: magic = 0x%08X, blocktotal = %d\n",
             sbdisk.sb_blocknum, sbdisk.sb_magic, sbdisk.sb_blocktotal);
        psuper->sb_blocknum = sbdisk.sb_blocknum;
        psuper->sb_blocktotal = sbdisk.sb_blocktotal;
        // Check for a valid free list.
        psuper->sb_freelst =
            (struct fbcnode *)sbOpenFreeList(diskfd,
                                               (int)sbdisk.sb_freelst);
        psuper->sb_dirlst =
            (struct dirblock *)sbOpenDirectory(diskfd,
                                               (int)sbdisk.sb_dirlst);
    }
    else
    {
        // Initialize disk.
        sbInit(psuper, diskfd);
    }

    return OK;
}
