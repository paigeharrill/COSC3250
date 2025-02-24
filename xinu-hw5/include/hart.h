/**
 * @file hart.h
 *
 * Contains functions for RISC-V Harts (hardware threads)
 */
/* Embedded Xinu, Copyright (C) 2024.  All rights reserved. */

#ifndef _HART_H_
#define _HART_H_

extern unsigned int gethartid(void);
extern unsigned long getmisa(void);
extern void set_satp(unsigned long);

#endif                          /* _HART_H_ */
