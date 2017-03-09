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
 * $Id: spl.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __spl_h__
#define __spl_h__

#include <sys/types.h>

typedef ulong spl_t;

inline static void
spl0()
{
	asm volatile 
	(
		"sti"
		:
		:
	);
}

inline static void
splhi()
{
	asm volatile
	(
		"cli"
		:
		:
	);
}

inline static spl_t
splhi_save()
{
	register spl_t ret;

	asm volatile
	(
		"pushfl\n\t"
		"popl %0\n\t"
		"cli"
		: "=r" (ret)
		:
	);
	return (ret & 0x200);
}

inline static void
splx(spl_t s)
{
	if(s) {
		asm volatile
		(
			"sti"
			:
			:
		);
	}
}

#endif /* __spl_h__ */
