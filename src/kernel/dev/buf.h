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
 * $Id: buf.h,v 1.1 1998/03/09 18:40:07 bart Exp $
 *
 */
 
#ifndef __buf_h__
#define __buf_h__

#include <sys/types.h>

struct buffer_head {
	ulong	dev;
	ulong	blk;
	ulong	flags;
	void*	data;
	struct	buffer_head* next;
	struct	buffer_head* prev;
	struct	buffer_head* next_free;
	struct 	buffer_head* prev_free;
};

int buf_init(ulong);
void buf_destroy();
struct buffer_head* bread(ulong dev, ulong blk);

#endif /* __buf_h__ */
