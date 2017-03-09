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
 * $Id: pit.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */
  
/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#ifndef __pit_h__
#define __pit_h__

#define TRES_PORT	(0x40)

/* 8254 programmable interrupt timer */
#define PITCTR0_PORT	(0x40)		/* counter 0 */
#define PITCTR1_PORT	(0x41)		/* counter 1 */
#define PITCTR2_PORT	(0x42)		/* counter 2 */

#define PITCTL_PORT	(0x43)		/* control port */
#define PITAUX_PORT	(0x61)		/* auxiliary port */

/* 8254 commands */

/* timer 0 */
#define PIT_C0		(0x00)		/* select counter 0 */
#define PIT_LOADMODE	(0x30)		/* load LSB followed by MSB */
#define PIT_NDIVMODE	(0x04)		/* divide by N counter */
#define PIT_SQUAREMODE	(0x06)		/* square-wave mode */

/* timer 1 */
#define PIT_C1		(0x40)		/* select counter 1 */
#define PIT_READMODE	(0x30)		/* read or load LSB, MSB */
#define PIT_RATEMODE	(0x06)		/* sqaure-wave mode for USART */

/* auxiliary control port for timer 2 */
#define PITAUX_GATE2	(0x01)		/* aux port, PIT gate 2 input */
#define PITAUX_OUT2	(0x02)		/* aux port, PIT clock out 2 enable */

#define CLKNUM	(1193167)

void init_pit();

#endif	/* __pit_h__ */

