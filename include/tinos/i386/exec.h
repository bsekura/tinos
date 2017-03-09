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
 * $Id: exec.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */


#ifndef __exec_h__
#define __exec_h__

#include <boot.h>

int exec_header(struct boot_task_s* b, struct exec_hdr_s* e);
void exec_user();

#endif /* __exec_h__ */ 