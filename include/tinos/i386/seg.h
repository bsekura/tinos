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
 *
 * $Id: seg.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */


/*
 * gdt selectors 
 */
#define GDTSEL_KCODE	(0x08)
#define GDTSEL_KDATA	(0x10)
#define GDTSEL_UCODE	(0x18)
#define GDTSEL_UDATA	(0x20)
#define GDTSEL_KTSS	(0x28)

#define USER_CSEG	(GDTSEL_UCODE | 0x03)
#define	USER_DSEG	(GDTSEL_UDATA | 0x03)

#define	KERN_BASE	(0x00000000)
#define USER_BASE	(0x40000000)
