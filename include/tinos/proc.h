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
 * $Id: proc.h,v 1.1.1.1 1998/02/26 19:01:22 bart Exp $
 *
 */

#ifndef __proc_h__
#define __proc_h__

#include <sys/types.h>
#include <vm/vas.h>
#include <thread.h>

struct process {
	ulong	pid;
	vas_desc_t*	vas;

	struct process* parent;

	struct tcb*	threads;

	ulong	brk;		/* current end of heap */
	ulong	highest;	/* highest virtual address allocated
				 * used to draw a boundary for 
				 * growing stack (down the vas) 
				 */
	ulong	stack;

	struct process* next;
	struct process* prev;
};

void 	init_proc();
ulong 	get_stack(struct process* p);
struct process* new_proc(struct process* parent);
int 	create_bootproc();

void proc_attach_thread(struct tcb* t);
void proc_detach_thread(struct tcb* t);

#endif /* __proc_h__ */
