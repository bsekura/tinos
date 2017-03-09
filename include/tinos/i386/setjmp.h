/* TINOS Operating System
 * Copyright (c) 1996, 1997 Bart Sekura
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and 
 * this permission notice appear in all copies of the software, 
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * $Id: setjmp.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

/*
 * setjmp, longjmp pair
 * this comes from VSTa
 * Copyright (c) 1996 Andy Valencia
 *
 */
#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <sys/types.h>

typedef struct {
	ulong eip;
	ulong edi;
	ulong esi;
	ulong ebp;
	ulong esp;
	ulong ebx;
	ulong edx;
	ulong ecx;
	ulong eax;
} jmp_buf[1];

/*
 * setjmp()
 *	Save context, returning 0
 */
inline static int
setjmp(jmp_buf regs)
{
	register int retcode;

	asm volatile 
	(
		"movl $1f,(%%edi)\n\t"
		"movl %%esi,8(%%edi)\n\t"
		"movl %%esp,%%eax\n\t"
		"movl %%ebp,12(%%edi)\n\t"
		"subl $4,%%eax\n\t"
		"movl %%eax,16(%%edi)\n\t"
		"xorl %%eax,%%eax\n\t"
		"movl %%ebx,20(%%edi)\n\t"
		"movl %%edx,24(%%edi)\n\t"
		"movl %%ecx,28(%%edi)\n"
		"1:\n\t"
		: "=a" (retcode)
		: "D" (regs)
	);

	return (retcode);
}

/*
 * longjmp()
 *	Restore context, returning a specified result
 */
inline static void
longjmp(jmp_buf regs, int retval)
{
	asm volatile
	(
		"movl 16(%%edi),%%esp\n\t"
		"movl 12(%%edi),%%ebp\n\t"
		"movl 8(%%edi),%%esi\n\t"
		"movl (%%edi),%%edx\n\t"
		"movl %%edx,(%%esp)\n\t"
		"movl 20(%%edi),%%ebx\n\t"
		"movl 24(%%edi),%%edx\n\t"
		"movl 28(%%edi),%%ecx\n\t"
		"sti\n\t"
		"ret\n\t"
		: /* No output */
		: "D" (regs), "a" (retval)
	);
}

#endif /* _SETJMP_H_ */
