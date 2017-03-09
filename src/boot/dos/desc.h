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
 * $Id: desc.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __desc_h__
#define __desc_h__

#include "types.h"

struct gate_desc {
   	ushort   offset_0;
   	ushort   selector;
   	ushort   type;
   	ushort   offset_16;
};

struct sys_desc {
   	ushort   limit;
   	ushort   base_0_15;
   	uchar    base_16_23;
   	uchar    dpl_type;
   	uchar    gav_lim;
   	uchar    base_24_31;
};

#endif /* __desc_h__ */
