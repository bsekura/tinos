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
 * $Id: tss.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __tss_h__
#define __tss_h__

#include <sys/types.h>

/* 
 * 80x86 task state segment
 * we don't use these for context switching
 * only needed for kernel to setup ring 0 stack (esp0 field)
 * (XXX is it necessary for anything else?)
 */

struct tss {
	ulong	link;
	ulong	esp0, ss0;
	ulong	esp1, ss1;
	ulong	esp2, ss2;
	ulong	cr3;
	ulong	eip;
	ulong	eflags;
	ulong	eax, ecx, edx, ebx, esp, ebp;
	ulong	esi, edi;
	ulong	es, cs, ss, ds, fs, gs;
	ulong	ldt;
	ushort 	trap;
	ushort	iomap;
};

#endif /* __tss_h__ */
