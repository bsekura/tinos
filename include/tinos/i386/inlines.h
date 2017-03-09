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
 * some of those are derived from VSTa's locore.h
 * Copyright (c) 1996 Andy Valencia
 *
 * $Id: inlines.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __inlines_h__
#define __inlines_h__

#include <sys/types.h>

inline static uchar
inb(int port)
{
	register uchar r;
   
   	asm volatile
	( 
		"inb %%dx, %%al\n\t" 
		: "=a" (r) 
		: "d" (port)
	);

   	return (r);
}

inline static void
outb(int port, uchar data)
{
	asm volatile
	(
		"outb %%al, %%dx\n\t" 
		:
		: "a" (data), "d" (port)
	);
}

inline static ushort
inw(int port)
{
	register ushort r;
   
   	asm volatile
	(
		"inw %%dx, %%ax\n\t" 
		: "=a" (r) 
		: "d" (port)
	);

   	return (r);
}

inline static void
outw(int port, ushort data)
{
	asm volatile
	(
		"outw %%ax, %%dx\n\t" 
		:
		: "a" (data), "d" (port)
	);
}

inline static void
repinsw(int port, void* va, int count)
{
   	asm volatile 
	(
		"rep\n\t"
               	"insw" 
		:
		: "d" (port), "D" (va), "c" (count)
		: "di"
	);
}  

inline static void
repoutsw(int port, void* va, int count)
{
   	asm volatile 
	(
		"rep\n\t"
               	"outsw" 
		:
		: "d" (port), "D" (va), "c" (count)
		: "di"
	);
}

inline static void
repinsl(int port, void* va, int count)
{
   	asm volatile 
	(
		"rep\n\t"
               	"insl" 
		:
		: "d" (port), "D" (va), "c" (count)
		: "di"
	);
}  

inline static void
repoutsl(int port, void* va, int count)
{
   	asm volatile 
	(
		"rep\n\t"
               	"outsl" 
		:
		: "d" (port), "D" (va), "c" (count)
		: "di"
	);
}

inline static void
sti()
{
	asm volatile 
	(
		"sti"
		:
		:
	);
}

inline static void
cli()
{
	asm volatile
	(
		"cli"
		:
		:
	);
}

inline static void
ltr(ulong val)
{
	asm volatile
	(
		"ltr %%eax\n\t"
		"jmp 1f\n\t"
		"1:\n\t" 
		:
		: "a" (val)
	);
}

inline static void
fast_copy(void* src, void* dest, int count)
{
	asm volatile
	(
		"rep\n\t"
               	"movsl"
               	:
		: "S" (src), "D" (dest), "c" (count)
	);
}

inline static void
set_cr3(ulong val)
{
	asm volatile
	(
		"movl %0, %%cr3\n\t" 
		: 
		: "r" (val)
	);
}

inline static ulong
get_cr3(void)
{
	register ulong r;
   
   	asm volatile
	( 
		"movl %%cr3, %0\n\t" 
		: "=r" (r) 
		:
	);

   	return (r);
}

inline static ulong
get_cr2(void)
{
	register ulong r;
   
   	asm volatile
	( 
		"movl %%cr2, %0\n\t" 
		: "=r" (r) 
		:
	); 

   	return(r);
}

inline static void
resume_current(void)
{
	asm volatile 
	(
      		"movl _cur_tcb, %%esp\n\t"
      		"popw %%es\n\t"
      		"popw %%ds\n\t"
      		"popal\n\t"
      		"iret\n\t"
      		:
		: 
	);
}

inline static void
stack_migrate(ulong esp, ulong eip)
{
	asm volatile
	(
		"movl %0, %%esp\n\t"
		"movl %1, (%%esp)\n\t"
		"sti\n\t"
		"ret"
		:
		: "r" (esp), "r" (eip)
	);
}

#endif /* __inlines_h__ */
