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
 * $Id: nvram.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __nvram_h__
#define __nvram_h__

#include <i386/inlines.h>

static inline int
nvread(int port)
{
	outb(0x70, port & 0xFF);
	return (inb(0x71) & 0xFF);
}

#define MEM_BASE_LO	(0x15)
#define MEM_BASE_HI	(0x16)
#define MEM_EXT_LO	(0x17)
#define MEM_EXT_HI	(0x18)

#endif /* __nvram_h__ */
