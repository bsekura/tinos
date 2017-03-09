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
 * $Id: multiboot.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __multiboot_h__
#define __multiboot_h__

#ifndef __mb_info_h__
#include <i386/mb_info.h>
#endif

int multiboot_init(struct multiboot_info*);
void multiboot_dump(struct multiboot_info*);

#endif /* __multiboot_h__ */
