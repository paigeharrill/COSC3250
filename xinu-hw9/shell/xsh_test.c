/**
 * @file     xsh_test.c
 * @provides xsh_test
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

void printFreeList(void);
/**
 * Shell command (test) is testing hook.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_test(int nargs, char *args[])
{
    //TODO: Test your O/S.
    //printf("This is where you should put some testing code.\n");
    int block = 0, i = 0;

    printFreeList();
    while(i < DISKBLOCKTOTAL){
    	block = sbGetBlock(supertab);
	printf("Block %d = %d\n", i, block);
	sbFreeBlock(supertab, block);
	printFreeList();
	if((i<5) || ((i>54) && (i<63))){
		int c = getc(CONSOLE);
	}
	i++;
    }
    return OK;
}
