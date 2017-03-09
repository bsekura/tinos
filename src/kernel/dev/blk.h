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
 * $Id: blk.h,v 1.1 1998/03/10 13:14:49 bart Exp $
 *
 */

#ifndef __blk_h__
#define __blk_h__
 
#include <sys/types.h>

void devblk_init();
int devblk_set_blksize(ulong dev_no, ulong blk_size);
int devblk_read(ulong dev_no, ulong block, ulong count, void* buf);

#endif /* __blk_h__ */
