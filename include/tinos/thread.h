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
 * $Id: thread.h,v 1.2 1998/02/26 19:46:26 bart Exp $
 *
 */


#ifndef __thread_h__
#define __thread_h__

#include <sys/types.h>
#include <i386/setjmp.h>
#include <i386/hat.h>
#include <vm/vas.h>


#define USERMODE(f)	((f)->ecs & 0x3)

/*
 * thread states
 */
#define TS_FREE 	(0x00)
#define TS_READY	(0x01)
#define TS_WAITING	(0x02)
#define TS_DEAD 	(0x03)

/*
 * some params
 */
#define DEFAULT_PRIO	(50)	/* default thread priority */
#define MAX_PRIO	(200)	/* max priority */
#define DEFAULT_QUANTA	(5)	/* default time quantum */
#define MIN_QUANTA	(2)	/* ...minimum */

struct process;

/*
 * thread control block
 */
struct tcb {
	jmp_buf ctx;		/* context */
	
	struct process*	proc;	/* process this thread belongs to */

	hat_desc_t* hat;	/* hat context */
	vas_desc_t* vas;	/* vas context */

	int	quantum;	/* quantum count */
	ulong	ticks;		/* ticks collected so far */
	int	prio;		/* base priority */
	int	cur_prio;	/* current priority */
	int	cnt;		/* current count (CPU usage) */
	ulong	stamp;
	int	preempt;

	ulong	tid;		/* thread id */
	ulong	pid;		/* process id thread belongs to */

	ulong	entry;		/* entry point */
	ulong	stk;		/* stack pointer */
	ulong	kstack; 	/* kernel stack pointer */
	
	struct tcb* next_proc;	/* process link */
	struct tcb* prev_proc;

	struct tcb* next;	/* scheduler link */
	struct tcb* prev;

	struct tcb* next_ready;	/* ready queue */
	struct tcb* prev_ready;

	struct tcb* s_next;	/* semaphore link */
	struct tcb* s_prev;

	ulong	s_wait; 	/* semaphore wait count */
	ulong	state;		/* state: ready, waiting and stuff */
};

struct tcb* create_thread(struct process* proc, ulong entry_addr);
void thread_execute(struct tcb*);

#endif /* __thread_h__ */

