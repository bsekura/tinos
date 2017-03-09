/* TINOS Operating System
 * Copyright (c) 1996, 1997, 1998 Bart Sekura
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
 *
 * $Id: syscall.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */
 
#include <i386/frame.h>
#include <i386/seg.h>

/*
#define ARG(x)	(*(p + x + 1))
#define	ARGP(x)	(*(p + x + 1) | USER_BASE)
*/

#define ARG(x)	(*(p + x))
#define	ARGP(x)	(*(p + x) | USER_BASE)

void
_sample_syscall(ulong stack)
{
	extern void sample_syscall(int, char*);
	struct frame* f = (struct frame*) &stack;
	ulong* p = (ulong*) (f->esp | USER_BASE);
#if 0
	int x = (int) *(p + 2);
	char* s = (char*) (*(p + 3) | USER_BASE);
#endif

	sample_syscall((int) ARG(1), (char*) ARGP(2));
}

void
_kbisr_syscall(ulong stack)
{
	extern int kbisr_syscall();
	struct frame* f = (struct frame*) &stack;
	
	f->eax = kbisr_syscall();
}
