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
 * programmable interrupt controller
 * 
 * $Id: pit.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

#include <pc/pit.h>
#include <i386/inlines.h>

#define HZ	(100)

void
init_pit()
{
#define clock (CLKNUM/HZ)
//#define clock ((CLKNUM + (HZ / 2)) / HZ)

	outb(PITCTL_PORT, PIT_C0 | PIT_SQUAREMODE | PIT_LOADMODE);
	outb(PITCTR0_PORT, clock & 0x00FF);
	outb(PITCTR0_PORT, (clock & 0xFF00) >> 8);
}
