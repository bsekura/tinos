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
 * $Id: sema.c,v 1.1.1.1 1998/02/26 19:01:21 bart Exp $
 *
 */

/*
 * semaphores are used to protect longer sequences of code manipulating
 * shared data and to sychronize threads. these semaphores are very simple
 * P, V interface. they should probably be inlined some day for better
 * efficiency
 *
 */

#include <i386/inlines.h>
#include <i386/spl.h>
#include <sema.h>
#include <thread.h>
#include <sched.h>
#include <queue.h>

extern struct tcb* cur_thread;

inline static void
sema_enq(semaphore_t* s, struct tcb* t)
{
	enqueue(s->wait_q, s_next, s_prev, t);
}

inline static void
sema_deq(semaphore_t* s, struct tcb* t)
{
	dequeue(s->wait_q, s_next, s_prev, t, struct tcb*);
}

void
sema_init(semaphore_t* s)
{
	spl_t i;

	i = splhi_save();
	s->count = 1;
	s->wait_q = NULL;
	splx(i);
}

void
sema_setval(semaphore_t* s, int val)
{
	spl_t i;

	i = splhi_save();
	if(s->wait_q) {
		printf("sema_setval(): warning, some thread(s) on wait_q.\n");
	}
	s->count = val;
	splx(i);
}

void
sema_p(semaphore_t* s)
{
	struct tcb* t;
	spl_t i;

	/* check the optimistic case first */
	i = splhi_save();
	s->count--;
	if(s->count >= 0) {
		splx(i);
		return;
	}

	/*
	 * enqueue current thread under the semaphore
	 * and put him to bed
	 */
	t = cur_thread;
	sema_enq(s, t);
	thread_sleep(t);
	t->s_wait++;
	splx(i);

	schedule();
}

void
sema_v(semaphore_t* s)
{
	struct tcb* t;
	spl_t i;

	/* if no threads kept, quickly bail out */
	i = splhi_save();
	s->count++;
	if(s->wait_q == NULL) {
		splx(i);
		return;
	}

	/*
	 * dequeue thread from under the semaphore
	 * and let it be woken up next timeslice
	 */
	t = s->wait_q;
	sema_deq(s, t);
	thread_wakeup(t);
	splx(i);
}

void
sema_dump(semaphore_t* s)
{
	struct tcb* t;
	spl_t i;

	printf("s->wait_q: ");
	i = splhi_save();
	t = s->wait_q;
	if(t) {
	      do {
		       printf("t[%08x],", (ulong) t);
		       t = t->s_next;
	      } while(t != s->wait_q);
	}
	splx(i);
	printf("\n");
}
